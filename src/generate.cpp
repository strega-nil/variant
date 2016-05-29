#include <iostream>
#include <cstdlib>
#include <sstream>

void boilerplate(std::ostream& os, std::size_t max) {
  if (max == 0) {
    return; // should throw
  }
  auto old_flags = os.flags();
  os << std::hex;
  os << "#include <type_traits>\n";
  os << "#include <cstdint>\n";
  os << "#include <functional>\n";

  os << "template<class... Ts> struct make_void{typedef void type;};\n";
  os << "template<class... Ts> using void_t = "
        "typename make_void<Ts...>::type;\n";
  os << "#define VARIANT_NARG__(...) "
        "VARIANT_NARG_I_(__VA_ARGS__, VARIANT_RSEQ_N())\n";
  os << "#define VARIANT_NARG_I_(...) VARIANT_ARG_N(__VA_ARGS__)\n";
  os << "#define VARIANT_ARG_N(NAME,";
  for (std::size_t i = 1; i <= max; ++i) {
    os << "_" << (i * 2 - 1) << ",_" << (i * 2) << ",";
  }
  os << "N, ...) N\n";


  os << "#define VARIANT_RSEQ_N() ";
  for (std::size_t i = max; i > 1; --i) {
    os << i << ",r,";
  }
  os << 1 << ",r\n";

  os << "#define _VFUNC_(NAME, N) NAME##N\n";
  os << "#define _VFUNC(NAME, N)  _VFUNC_(NAME, N)\n";
  os << "#define VFUNC(FUNC, ...) "
        "_VFUNC(FUNC, VARIANT_NARG__(__VA_ARGS__)) (__VA_ARGS__)\n";

  os << "#define end_adt() };\n";
  os << "#define adt(NAME, ...) VFUNC(adt, NAME, __VA_ARGS__)\n";
  os << "#define adtr(NAME, ...) "
    "enum error_incorrect_number_of_arguments_to_adt_for_##NAME;\n";

  os << "#define _impl_adt_real(T) "
    "std::conditional_t<std::is_void<T>::value, std::uint8_t, T>\n";
  os << "\n";
  os.setf(old_flags);
}

void build_implementation(std::ostream& os, std::size_t current) {
  if (current == 0) {
    return;
  }
  auto old_flags = os.flags();
  os << std::hex;
  std::stringstream args_ss;
  std::stringstream call_ss;
  args_ss << std::hex << current << "(NAME";
  call_ss << std::hex << current - 1 << "(NAME";
  for (std::size_t i = 0; i < current; ++i) {
    args_ss << ",V" << i + 1 << ",T" << i + 1;
  }
  for (std::size_t i = 0; i < current - 1; ++i) {
    call_ss << ",V" << i + 1 << ",T" << i + 1;
  }
  args_ss << ")";
  call_ss << ")";
  auto args = args_ss.str();
  auto call = call_ss.str();

  std::stringstream type_ss;
  std::stringstream variant_ss;
  type_ss << std::hex << "T" << current;
  variant_ss << std::hex << "V" << current;
  auto type = type_ss.str();
  auto var = variant_ss.str();

  // first: the definition of kind
  os << "#define _impl_adt_K" << args << " ";
  if (current == 1) {
  } else {
    os << "_impl_adt_K" << call;
  }
  os << var << ",\n";

  // second: the definition of the union

  // Some notes:
  // 1) the destructor is guaranteed to be a no op
  // 2) copies are guaranteed to be shallow
  // you *must* do the interesting stuff in the outer layer; that's why data is
  //   private
  os << "#define _impl_adt_D" << args;
  if (current == 1) {
    os << "data(){std::memset(reinterpret_cast<void*>(this),0,sizeof(*this));}";
    os << "~data(){}";
    os << "data(data const& d){";
    os <<   "std::memcpy(reinterpret_cast<void*>(this),";
    os <<     "reinterpret_cast<void const*>(&d), sizeof(*this));";
    os << "}";
    os << "data(data&& d){";
    os <<   "std::memcpy(reinterpret_cast<void*>(this),";
    os <<     "reinterpret_cast<void const*>(&d), sizeof(*this));";
    os << "}";
    os << "data& operator=(data const& d){new(this) data(d);return *this;}";
    os << "data& operator=(data&& d){new(this) data(d);return *this;}";
    os << "";
  } else {
    os << "_impl_adt_D" << call;
  }
  os << "_impl_adt_real(" << type << ")" << var << ";\n";

  // third: the definition of the matcher functions
  os << "#define _impl_adt_MF" << args << " ";
  if (current == 1) {
    os << "static void _no_op(){}";
    os << "std::function<void()>default_=_no_op;";
  } else {
    os << "_impl_adt_MF" << call;
  }
  os << "std::function<void(" << type << ")>" << var << "##_;\n";

  // fourth: the definition of the matcher methods
  os << "#define _impl_adt_MM" << args << " ";
  if (current == 1) {
    os << "private:";
    os << "template<class t,class...args>";
    os << "static void _call_func(std::function<void(args...)>,t);";
    os << "template<class t>";
    os << "static void _call_func(std::function<void()>f,t){f();}";
    os << "template<class t>";
    os << "static void _call_func(std::function<void(t)>f,t g){f(g);}";
    os << "public:";
    os << "matcher(){}";
    os << "matcher const& _(std::function<void()>f){";
    os <<   "default_=f;";
    os <<   "return *this;";
    os << "}";
  } else {
    os << "_impl_adt_MM" << call;
  }
  os << "template<class T=" << type << ">";
  os << "void call_##" << var << "(_impl_adt_real(" << type << ")a)const{";
  os <<   "_call_func(" << var << "##_, a);";
  os << "}";
  os << "matcher& " << var << "(std::function<void(" << type << ")>f){";
  os <<   var << "##_=f;";
  os <<   "return *this;";
  os << "}\n";

  // fifth: the definition of the methods
  os << "#define _impl_adt_M" << args << " ";
  if (current == 1) {
    // unsafe to call constructor, as if it were destructed,
    // the variant would be destroyed without ever being constructed
    os << "private:" "NAME(kind k):kind_(k){}" "public:";
  } else {
    os << "_impl_adt_M" << call;
  }
  // NOTE: these enable_ifs are here to make certain that
  // the copy and move "constructors" are only callable on data variants, while
  // the "default constructors" are only callable on void variants
  os << "template<class T=" << type << ",";
  os <<   "class=std::enable_if_t<!std::is_void<T>::value>>";
  os << "static NAME " << var << "(_impl_adt_real(" << type << ")const& t){";
  os <<   "NAME ret(kind::" << var << ");";
  os <<   "try{new(&ret.data_." << var << ")T(t);}catch(...){std::terminate();}";
  os <<   "return ret;";
  os << "}";
  os << "template<class T=" << type << ",";
  os <<   "class=std::enable_if_t<!std::is_void<T>::value>>";
  os << "static NAME " << var << "(_impl_adt_real(" << type << ")&& t){";
  os <<   "NAME ret(kind::" << var << ");";
  os <<   "try{new(&ret.data_." << var << ")T(";
  os <<       "std::move(t));}catch(...){std::terminate();}";
  os <<   "return ret;";
  os << "}";
  os << "template<class T=" << type;
  os <<   ",class=std::enable_if_t<std::is_void<T>::value>>";
  os << "static NAME " << var << "(){";
  os <<   "NAME ret(kind::" << var << ");";
  os <<   "return ret;";
  os << "}\n";

  // sixth: the match switch on the adt
  os << "#define _impl_adt_MS" << args << " ";
  if (current == 1) {
  } else {
    os << "_impl_adt_MS" << call;
  }
  os << "case kind::" << var << ":m.call_##" << var << "(data_." << var << ");"
      "break;\n";

  // seventh: the destructor
  os << "#define _impl_adt_Ds" << args << " ";
  if (current == 1) {
  } else {
    os << "_impl_adt_Ds" << call;
  }
  os << "case kind::" << var << ":data_." << var
      << ".~decltype(data_." << var << ")();break;\n";

  // eighth: the copy constructor
  os << "#define _impl_adt_Cc" << args << " ";
  if (current == 1) {
  } else {
    os << "_impl_adt_Cc" << call;
  }
  os << "case kind::" << var << ":";
  os <<   "new(&data_." << var << ")";
  os <<     "decltype(data_." << var << ")(a.data_." << var << ");";
  os <<   "break;\n";

  // ninth: the move constructor
  os << "#define _impl_adt_Mc" << args << " ";
  if (current == 1) {
  } else {
    os << "_impl_adt_Mc" << call;
  }
  os << "case kind::" << var << ":";
  os <<   "new(&data_." << var << ")";
  os <<     "decltype(data_." << var << ")(std::move(a.data_." << var << "));";
  os <<   "break;\n";

  // tenth: the definition itself
  os << "#define adt" << args << " ";
  os << "class NAME {";
  os << "public:";
  os <<   "enum class kind{";
  os <<     "_impl_adt_K" << args;
  os <<   "};";
  os << "private:";
  os <<   "kind kind_;";
  os <<   "union data{";
  os <<     "_impl_adt_D" << args;
  os <<   "}data_;";
  os << "public:";
  os <<   "class matcher{";
  os <<     "_impl_adt_MF" << args;
  os <<   "public:";
  os <<     "_impl_adt_MM" << args;
  os <<   "};";
  os <<   "friend void swap(NAME& l, NAME& r){";
  os <<     "std::swap(l.kind_,r.kind_);";
  os <<     "std::swap(l.data_,r.data_);";
  os <<   "}";
  os <<   "NAME(NAME const& a):NAME(a.kind_){";
  os <<     "switch(kind_){_impl_adt_Cc" << args << "}";
  os <<   "}";
  os <<   "NAME(NAME&& a):NAME(a.kind_){";
  os <<     "switch(kind_){_impl_adt_Mc" << args << "}";
  os <<   "}";
  os <<   "NAME& operator=(NAME o){";
  os <<     "swap(*this,o);";
  os <<     "return *this;";
  os <<   "}";
  os <<   "~NAME(){";
  os <<     "switch(kind_){_impl_adt_Ds" << args << "}";
  os <<   "}";
  os <<   "_impl_adt_M" << args;
  os <<   "void match(matcher const& m){";
  os <<     "switch(kind_){_impl_adt_MS" << args << "}";
  os <<   "}";
  os << "private:" << "\n";
  os.setf(old_flags);
}

int main() {
  std::size_t max = 128;
  boilerplate(std::cout, max);
  for (std::size_t i = 1; i <= max; ++i) {
    build_implementation(std::cout, i);
  }
}
