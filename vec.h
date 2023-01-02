//
// Created by 王泽远 on 2023/1/2.
//

#ifndef MONTECARLO_VEC_H
#define MONTECARLO_VEC_H

#include <array>

template<typename T,unsigned int dim = 1>
struct basic_vec
{
    std::array<T,dim> val{};

    basic_vec()= default;

    explicit basic_vec(const std::array<T,dim>& arr){
        val = arr;
    }

    explicit basic_vec(std::array<T,dim>&& arr){
        val = arr;
    }

    static auto zeros() {
        return basic_vec(std::array<T,dim>{});
    }

    static auto ones() {
        std::array<T,dim> arr;
        arr.fill(static_cast<T>(1));
        return basic_vec(arr);
    }

    T operator[](unsigned int n) const noexcept{
        return val[n];
    }

    auto operator+() const noexcept{

    }

    auto operator-()const noexcept{

    }

    void operator+=(const basic_vec<T,dim> &other) noexcept{

    }

    void operator-=(const basic_vec<T,dim> &other) noexcept{

    }

    void operator*=(const T & scalar) noexcept{

    }

    void operator/=(const T& scalar) noexcept{

    }

};

template<typename T>
struct basic_vec<T,2>
{
    T x,y;

    basic_vec<T,2>()= default;

    basic_vec<T,2>(const T& _x,const T& _y){
        x = _x;
        y = _y;
    }

    basic_vec<T,2>(T&& _x,T&& _y){
        x = _x;
        y = _y;
    }

    explicit basic_vec<T,2>(const std::array<T,2>& arr){
        basic_vec(arr[0],arr[1]);
    }

    explicit basic_vec<T,2>(std::array<T,2>&& arr){
        basic_vec(arr[0],arr[1]);
    }

    static basic_vec<T,2> zeros() {
        return basic_vec<T,2>(static_cast<T>(0),static_cast<T>(0));
    }

    static basic_vec<T,2> ones() {
        return basic_vec(static_cast<T>(1),static_cast<T>(1));
    }

    T operator[](unsigned int n) const noexcept{
        //return n==0?x:y;
    }

    auto operator+() const noexcept{

    }

    auto operator-()const noexcept{

    }

    void operator+=(const basic_vec<T,2> &other) noexcept{

    }

    void operator-=(const basic_vec<T,2> &other) noexcept{

    }
};

template<typename T>
struct basic_vec<T,3>
{
    T x,y,z;

    basic_vec<T,3>()= default;

    basic_vec<T,3>(const T& _x,const T& _y,const T& _z){
        x = _x;
        y = _y;
        z = _z;
    }

    basic_vec<T,2>(T&& _x,T&& _y,T&& _z){
        x = _x;
        y = _y;
        z = _z;
    }

    explicit basic_vec<T,3>(const std::array<T,3>& arr){
        basic_vec<T,3>(arr[0],arr[1],arr[2]);
    }

    explicit basic_vec<T,3>(std::array<T,3>&& arr){
        basic_vec<T,3>(arr[0],arr[1],arr[2]);
    }

    static basic_vec<T,3> zeros() {
        return basic_vec<T,3>(static_cast<T>(0),static_cast<T>(0),static_cast<T>(0));
    }

    static basic_vec<T,3> ones() {
        return basic_vec<T,3>(static_cast<T>(1),static_cast<T>(1),static_cast<T>(1));
    }

    T operator[](unsigned int n) const noexcept{

    }

    auto operator+() const noexcept{

    }

    auto operator-()const noexcept{

    }

    void operator+=(const basic_vec<T,2> &other) noexcept{

    }

    void operator-=(const basic_vec<T,2> &other) noexcept{

    }
};

template<typename T>
struct basic_vec<T,4>
{
    T x,y,z,w;

    basic_vec<T,4>()= default;

    basic_vec<T,4>(const T& _x,const T& _y,const T& _z,const T& _w){
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    basic_vec<T,2>(T&& _x,T&& _y,T&& _z,T&&_w){
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    explicit basic_vec<T,4>(const std::array<T,4>& arr){
        basic_vec<T,4>(arr[0],arr[1],arr[2],arr[3]);
    }

    explicit basic_vec<T,4>(std::array<T,4>&& arr){
        basic_vec<T,4>(arr[0],arr[1],arr[2],arr[3]);
    }

    static basic_vec<T,4> zeros() {
        return basic_vec<T,4>(static_cast<T>(0),static_cast<T>(0),static_cast<T>(0),static_cast<T>(0));
    }

    static basic_vec<T,4> ones() {
        return basic_vec<T,3>(static_cast<T>(1),static_cast<T>(1),static_cast<T>(1),static_cast<T>(1));
    }

    T operator[](unsigned int n) const noexcept{

    }

    auto operator+() const noexcept{

    }

    auto operator-()const noexcept{

    }

    void operator+=(const basic_vec<T,2> &other)const noexcept{

    }

    void operator-=(const basic_vec<T,2> &other)const noexcept{

    }
};

template<unsigned int dim>
using vec = basic_vec<double,dim>;

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

#endif //MONTECARLO_VEC_H
