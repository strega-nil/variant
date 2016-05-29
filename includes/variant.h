#include <type_traits>
#include <cstdint>
#include <functional>
template<class... Ts> struct make_void{typedef void type;};
template<class... Ts> using void_t = typename make_void<Ts...>::type;
#define VARIANT_NARG__(...) VARIANT_NARG_I_(__VA_ARGS__, VARIANT_RSEQ_N())
#define VARIANT_NARG_I_(...) VARIANT_ARG_N(__VA_ARGS__)
#define VARIANT_ARG_N(NAME,_1,_2,_3,_4,N, ...) N
#define VARIANT_RSEQ_N() 2,r,1,r
#define _VFUNC_(NAME, N) NAME##N
#define _VFUNC(NAME, N)  _VFUNC_(NAME, N)
#define VFUNC(FUNC, ...) _VFUNC(FUNC, VARIANT_NARG__(__VA_ARGS__)) (__VA_ARGS__)
#define end_adt() };
#define adt(NAME, ...) VFUNC(adt, NAME, __VA_ARGS__)
#define adtr(NAME, ...) enum error_incorrect_number_of_arguments_to_adt_for_##NAME;
#define _impl_adt_real(T) std::conditional_t<std::is_void<T>::value, std::uint8_t, T>

#define _impl_adt_K1(NAME,V1,T1) V1,
#define _impl_adt_D1(NAME,V1,T1)data(){std::memset(reinterpret_cast<void*>(this),0,sizeof(*this));}~data(){}data(data const& d){std::memcpy(reinterpret_cast<void*>(this),reinterpret_cast<void const*>(&d), sizeof(*this));}data(data&& d){std::memcpy(reinterpret_cast<void*>(this),reinterpret_cast<void const*>(&d), sizeof(*this));}data& operator=(data const& d){new(this) data(d);return *this;}data& operator=(data&& d){new(this) data(d);return *this;}_impl_adt_real(T1)V1;
#define _impl_adt_MF1(NAME,V1,T1) static void _no_op(){}std::function<void()>default_=_no_op;std::function<void(T1)>V1##_;
#define _impl_adt_MM1(NAME,V1,T1) private:template<class t,class...args>static void _call_func(std::function<void(args...)>,t);template<class t>static void _call_func(std::function<void()>f,t){f();}template<class t>static void _call_func(std::function<void(t)>f,t g){f(g);}public:matcher(){}matcher const& _(std::function<void()>f){default_=f;return *this;}template<class T=T1>void call_##V1(_impl_adt_real(T1)a)const{_call_func(V1##_, a);}matcher& V1(std::function<void(T1)>f){V1##_=f;return *this;}
#define _impl_adt_M1(NAME,V1,T1) private:NAME(kind k):kind_(k){}public:template<class T=T1,class=std::enable_if_t<!std::is_void<T>::value>>static NAME V1(_impl_adt_real(T1)const& t){NAME ret(kind::V1);try{new(&ret.data_.V1)T(t);}catch(...){std::terminate();}return ret;}template<class T=T1,class=std::enable_if_t<!std::is_void<T>::value>>static NAME V1(_impl_adt_real(T1)&& t){NAME ret(kind::V1);try{new(&ret.data_.V1)T(std::move(t));}catch(...){std::terminate();}return ret;}template<class T=T1,class=std::enable_if_t<std::is_void<T>::value>>static NAME V1(){NAME ret(kind::V1);return ret;}
#define _impl_adt_MS1(NAME,V1,T1) case kind::V1:m.call_##V1(data_.V1);break;
#define _impl_adt_Ds1(NAME,V1,T1) case kind::V1:data_.V1.~decltype(data_.V1)();break;
#define _impl_adt_Cc1(NAME,V1,T1) case kind::V1:new(&data_.V1)decltype(data_.V1)(a.data_.V1);break;
#define _impl_adt_Mc1(NAME,V1,T1) case kind::V1:new(&data_.V1)decltype(data_.V1)(std::move(a.data_.V1));break;
#define adt1(NAME,V1,T1) class NAME {public:enum class kind{_impl_adt_K1(NAME,V1,T1)};private:kind kind_;union data{_impl_adt_D1(NAME,V1,T1)}data_;public:class matcher{_impl_adt_MF1(NAME,V1,T1)public:_impl_adt_MM1(NAME,V1,T1)};friend void swap(NAME& l, NAME& r){std::swap(l.kind_,r.kind_);std::swap(l.data_,r.data_);}NAME(NAME const& a):NAME(a.kind_){switch(kind_){_impl_adt_Cc1(NAME,V1,T1)}}NAME(NAME&& a):NAME(a.kind_){switch(kind_){_impl_adt_Mc1(NAME,V1,T1)}}NAME& operator=(NAME o){swap(*this,o);return *this;}~NAME(){switch(kind_){_impl_adt_Ds1(NAME,V1,T1)}}_impl_adt_M1(NAME,V1,T1)void match(matcher const& m){switch(kind_){_impl_adt_MS1(NAME,V1,T1)}}private:
#define _impl_adt_K2(NAME,V1,T1,V2,T2) _impl_adt_K1(NAME,V1,T1)V2,
#define _impl_adt_D2(NAME,V1,T1,V2,T2)_impl_adt_D1(NAME,V1,T1)_impl_adt_real(T2)V2;
#define _impl_adt_MF2(NAME,V1,T1,V2,T2) _impl_adt_MF1(NAME,V1,T1)std::function<void(T2)>V2##_;
#define _impl_adt_MM2(NAME,V1,T1,V2,T2) _impl_adt_MM1(NAME,V1,T1)template<class T=T2>void call_##V2(_impl_adt_real(T2)a)const{_call_func(V2##_, a);}matcher& V2(std::function<void(T2)>f){V2##_=f;return *this;}
#define _impl_adt_M2(NAME,V1,T1,V2,T2) _impl_adt_M1(NAME,V1,T1)template<class T=T2,class=std::enable_if_t<!std::is_void<T>::value>>static NAME V2(_impl_adt_real(T2)const& t){NAME ret(kind::V2);try{new(&ret.data_.V2)T(t);}catch(...){std::terminate();}return ret;}template<class T=T2,class=std::enable_if_t<!std::is_void<T>::value>>static NAME V2(_impl_adt_real(T2)&& t){NAME ret(kind::V2);try{new(&ret.data_.V2)T(std::move(t));}catch(...){std::terminate();}return ret;}template<class T=T2,class=std::enable_if_t<std::is_void<T>::value>>static NAME V2(){NAME ret(kind::V2);return ret;}
#define _impl_adt_MS2(NAME,V1,T1,V2,T2) _impl_adt_MS1(NAME,V1,T1)case kind::V2:m.call_##V2(data_.V2);break;
#define _impl_adt_Ds2(NAME,V1,T1,V2,T2) _impl_adt_Ds1(NAME,V1,T1)case kind::V2:data_.V2.~decltype(data_.V2)();break;
#define _impl_adt_Cc2(NAME,V1,T1,V2,T2) _impl_adt_Cc1(NAME,V1,T1)case kind::V2:new(&data_.V2)decltype(data_.V2)(a.data_.V2);break;
#define _impl_adt_Mc2(NAME,V1,T1,V2,T2) _impl_adt_Mc1(NAME,V1,T1)case kind::V2:new(&data_.V2)decltype(data_.V2)(std::move(a.data_.V2));break;
#define adt2(NAME,V1,T1,V2,T2) class NAME {public:enum class kind{_impl_adt_K2(NAME,V1,T1,V2,T2)};private:kind kind_;union data{_impl_adt_D2(NAME,V1,T1,V2,T2)}data_;public:class matcher{_impl_adt_MF2(NAME,V1,T1,V2,T2)public:_impl_adt_MM2(NAME,V1,T1,V2,T2)};friend void swap(NAME& l, NAME& r){std::swap(l.kind_,r.kind_);std::swap(l.data_,r.data_);}NAME(NAME const& a):NAME(a.kind_){switch(kind_){_impl_adt_Cc2(NAME,V1,T1,V2,T2)}}NAME(NAME&& a):NAME(a.kind_){switch(kind_){_impl_adt_Mc2(NAME,V1,T1,V2,T2)}}NAME& operator=(NAME o){swap(*this,o);return *this;}~NAME(){switch(kind_){_impl_adt_Ds2(NAME,V1,T1,V2,T2)}}_impl_adt_M2(NAME,V1,T1,V2,T2)void match(matcher const& m){switch(kind_){_impl_adt_MS2(NAME,V1,T1,V2,T2)}}private:
