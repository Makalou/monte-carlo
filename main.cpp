#include <iostream>
#include <cmath>
#include <random>
#include "integrator.h"

#ifdef SHOW_IMG
#include "matplot/matplot.h"
namespace plt = matplot;
#endif

my_float f(my_float x){
    const static my_float a = -0.24;
    const static my_float b = 3.7;
    const static my_float pi = 3.1415926535897;
    my_float sum = 0;
    for(int n =0;n<=10;++n){
        sum += std::pow(a,n)*std::sin(0.1*std::pow(b,n)*pi*x);
    }
    return 10.0*sum;
}

my_float g(my_float x){
    return x*x;
}

class my_sampler : public sampler{
    my_float a;
    my_float b;
    std::uniform_real_distribution<double> dis;
    std::mt19937 gen;
public:

    explicit my_sampler(){
        std::random_device rd;  // Will be used to obtain a seed for the random number engine
        gen = std::mt19937(rd());
    }

    void set_domain(my_float _a, my_float _b) override{
        a = _a;
        b = _b;
        dis = std::uniform_real_distribution<my_float>(a,b);
    }

    my_float next_sample() override{
        return dis(gen);
    }

    my_float evaluate(my_float sample) const override{
        return 1.0/(b-a);
    }
};

int main() {

    auto ground_truth = integrator(make_integral_info(f,0,10), 1e6);
    printf("%.8f\n", ground_truth);
    my_sampler s{};
    while (true){
        printf("%.8f\n",ris_monte_carlo_integrator(make_integral_info(f,0,10),s,f,1));
    }

#ifdef SHOW_IMG
    auto ground_truth_square = ground_truth * ground_truth;
    std::vector<std::vector<my_float>> img1;
    std::vector<std::vector<my_float>> img2;
//#define IMG1
#ifdef IMG1
    for(int i = 0;i<100;++i){
        img1.emplace_back();
        for(int j=0;j<100;++j){
            auto pixel = naive_monte_carlo_integrator(f,0,10,s);
            pixel = (pixel-ground_truth)*(pixel-ground_truth)/ground_truth_square;
            img1[i].emplace_back(pixel*255.0);
        }
    }

    plt::image(std::make_tuple(img1,plt::zeros(100,100),plt::zeros(100,100)));
    plt::show();

#else
    for(int i = 0;i<100;++i){
        img2.emplace_back();
        for(int j=0;j<100;++j){
            auto pixel = ris_monte_carlo_integrator(f,0,10,s,f);
            pixel = (pixel-ground_truth)*(pixel-ground_truth)/ground_truth_square;
            img2[i].emplace_back(pixel*255.0);
        }
    }


    plt::image(std::make_tuple(img2,plt::zeros(100,100),plt::zeros(100,100)));
    plt::show();
#endif
#endif
    return 0;
}
