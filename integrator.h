//
// Created by 王泽远 on 2022/12/29.
//

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <cmath>
#include <functional>
#include <random>
#include <numeric>
#include <utility>

using my_float = double;
using integrand = std::function<my_float (my_float)>;

template<typename T>
using domain = std::pair<T,T>;

using integral_info = std::pair<integrand,domain<my_float>>;

integral_info make_integral_info(const integrand& ig,my_float a, my_float b){
    return std::make_pair(ig,std::make_pair(a,b));
}

/*
 * Trapezoid integrator
 * Caution: fp swamping issue
 */
my_float integrator(const integral_info& int_info,const int steps){
    const auto & dom = int_info.second;
    const auto & f = int_info.first;

    auto interval = dom.second-dom.first;
    auto dx = interval/steps;
    my_float res = 0;
    my_float a1;
    my_float a2 = f(dom.first);

    for(int i = 1;i<=steps;++i){
        a1 = a2;
        a2 = f(dom.first+dx*i);
        res += (a1+a2)*dx*0.5;
    }

    return res;
}

class sampler{
public:
    virtual void set_domain(my_float a,my_float b) = 0;

    virtual my_float next_sample() = 0;

    virtual my_float evaluate(my_float sample) const = 0;
};

using sample_info = std::pair<sampler&,const int>;

sample_info make_sample_info(sampler& _sampler, const int N){
    return std::make_pair(std::ref(_sampler),N);
}

/*
 * Most naive monte carlo integrator
 * A special case of MIS MC integrator(with w(x) == 1)
 */
my_float naive_monte_carlo_integrator(const integral_info & int_info,sample_info s_info){
    const auto N = s_info.second;
    auto & _sampler = s_info.first;

    _sampler.set_domain(int_info.second.first,int_info.second.second);

    const auto & f = int_info.first;

    my_float result{};
    int i = 1;
    for(;i<=N;++i){
        auto x = _sampler.next_sample();
        auto pdf = _sampler.evaluate(x);
        result += (f(x)/pdf - result )/my_float(i);
    }
    return result;
}

/*
 * Monte carlo integrator with Multiple Importance Sampling
 */
my_float mis_monte_carlo_integrator(const integral_info & int_info,std::vector<sample_info>& sample_infos){

    my_float result{};

    for(auto & si : sample_infos){
        si.first.set_domain(int_info.second.first,int_info.second.second);
    }

    const auto & f = int_info.first;

    for(auto & si : sample_infos){
        const auto Ni = si.second;
        const auto inv_float_Ni = 1.0/static_cast<my_float>(Ni);

        auto& sampler_i = si.first;

        const auto wi = [&sample_infos,Ni,&sampler_i](my_float sample)->my_float {
            //balance heuristic
            my_float sum{};
            for(auto & sj : sample_infos){
                sum += sj.second * sj.first.evaluate(sample);
            }
            return sampler_i.evaluate(sample)*Ni/sum;
        };

        for(int j =0; j < Ni; ++j){
            auto xij = sampler_i.next_sample();
            auto pdf_i = sampler_i.evaluate(xij);
            result += f(xij)/pdf_i * inv_float_Ni * wi(xij);
        }
    }

    return result;
}

/*
 * Sample Importance Resampling
 */

struct sir_sampler{
private:
    std::uniform_real_distribution<my_float> dice;
    std::mt19937 gen;
    std::vector<my_float> _proposal_set;
    std::vector<my_float> _prefix_sum;
    my_float correct_factor;
public:
    sir_sampler(sampler& proposal_sampler,const std::function<my_float(my_float)>& target_distribution,const int M){

        static std::random_device rd;  // Will be used to obtain a seed for the random number engine
        gen =  std::mt19937(rd());
        dice = std::uniform_real_distribution<my_float>(0,1);

        const auto inv_float_M = 1.0/static_cast<my_float>(M);
        std::vector<my_float> weight_set{};
        _proposal_set.reserve(M);
        weight_set.reserve(M);

        //generate M proposal samples, and calculate weights for each.
        for(int j = 0; j<M; ++j){
            auto xj = proposal_sampler.next_sample();
            _proposal_set.emplace_back(xj);
            auto p_hat = target_distribution(xj);
            auto p = proposal_sampler.evaluate(xj);
            auto w = (p_hat/p)*inv_float_M;
            weight_set.emplace_back(w);
        }

        correct_factor = std::accumulate(weight_set.begin(),weight_set.end(),0.0);

        _prefix_sum.clear();
        std::inclusive_scan(weight_set.cbegin(),weight_set.cend(),std::back_inserter(_prefix_sum),std::plus<my_float>{});
        auto inv_total_sum = 1.0/_prefix_sum.back();
        std::for_each(_prefix_sum.begin(),_prefix_sum.end(),[inv_total_sum](auto & s){s *= inv_total_sum;});
    }

    my_float next_sample(){
        auto rand = dice(gen);
        //use binary search to draw a new weighted sample
        auto lower = std::lower_bound(_prefix_sum.begin(),_prefix_sum.end(),rand);
        return _proposal_set[std::distance(_prefix_sum.begin(),lower)];
    }

    my_float get_correct_factor() const{
        return correct_factor;
    }
};

/*
 * Weighted Reservoir Sampling
 */
struct reservoir{
    my_float current_sample{};
    my_float total_weight{};
};

struct wrs_sampler{
private:
    std::uniform_real_distribution<my_float> dice;
    std::mt19937 gen;
    reservoir _reservoir{};
    std::function<my_float(my_float)> _target_distribution;
    std::shared_ptr<sampler> _proposal_sampler;
    int M = 0;
public:
    wrs_sampler(std::shared_ptr<sampler> proposal_sampler,const std::function<my_float(my_float)>& target_distribution){

        static std::random_device rd;  // Will be used to obtain a seed for the random number engine
        gen =  std::mt19937(rd());
        dice = std::uniform_real_distribution<my_float>(0,1);
        _target_distribution = target_distribution;
        _proposal_sampler = std::move(proposal_sampler);
    }

    my_float next_sample(){
        auto x = _proposal_sampler->next_sample();
        auto p = _proposal_sampler->evaluate(x);
        auto p_hat = _target_distribution(x);
        auto w = p_hat/p;

        if(M == 0){
            _reservoir.current_sample = x;
        }else{
            auto threshold = w/(w+_reservoir.total_weight);
            auto rand = dice(gen);
            if(rand < threshold){
                _reservoir.current_sample = x;
            }
        }

        _reservoir.total_weight += w;
        M++;

        return _reservoir.current_sample;
    }

    my_float get_correct_factor() const{
        return _reservoir.total_weight/M;
    }
};

/*
 * Monte carlo integrator with Resampled Importance Sampling
 */
my_float ris_monte_carlo_integrator(const integral_info & int_info,sampler& proposal_sampler,const std::function<my_float(my_float)>& target_distribution,const int N){

    proposal_sampler.set_domain(int_info.second.first,int_info.second.second);

    sir_sampler sir{proposal_sampler,target_distribution,1000};

    const auto & f = int_info.first;

    my_float result{};
    for(int i = 1; i<=N; ++i){
        auto x = sir.next_sample();
        result += (f(x)/target_distribution(x) - result)/my_float(i);
    }

    result *= sir.get_correct_factor();

    return result;
}

/*
 * Monte carlo integrator with Weighted Reservoir Sampling
 * Has side effect on WR sampler.
 */
my_float wrs_monte_carlo_integrator(const integral_info & int_info,wrs_sampler& wrs,const std::function<my_float(my_float)>& target_distribution,const int N){
    const auto inv_float_N = 1.0/static_cast<my_float>(N);

    const auto & f = int_info.first;

    my_float result{};
    for(int  i = 0; i<N; ++i){
        auto x = wrs.next_sample();
        result += f(x)/target_distribution(x) * inv_float_N;
    }

    result *= wrs.get_correct_factor();

    return result;
}
#endif //INTEGRATOR_H
