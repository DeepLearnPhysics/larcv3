#ifndef __LARCV3BASE_PSET_CXX
#define __LARCV3BASE_PSET_CXX

#include "larcv3/core/base/PSet.h"
#include <sstream>
namespace larcv3 {

  PSet::PSet(const std::string name,
             const std::string data)
  {
    if(name.empty()) {
      LARCV_CRITICAL() << "Cannot make PSet with an empty name!" << std::endl;
      throw larbys("Cannot make PSet with an empty name!");
    }
    _name = name;
    if(!data.empty()) {
      this->add(data);
    }
  }

  size_t PSet::size() const
  { return (_data_value.size() + _data_pset.size()); }

  const std::vector<std::string> PSet::keys() const
  {
    std::vector<std::string> res;
    res.reserve(_data_value.size() + _data_pset.size());
    for(auto const& key_value : _data_value) res.push_back(key_value.first);
    for(auto const& key_value : _data_pset ) res.push_back(key_value.first);
    return res;
  }
  
  const std::vector<std::string> PSet::value_keys () const
  {
    std::vector<std::string> res;
    res.reserve(_data_value.size());
    for(auto const& key_value : _data_value) res.push_back(key_value.first);
    return res;
  }
  const std::vector<std::string> PSet::pset_keys  () const
  {
    std::vector<std::string> res;
    res.reserve(_data_pset.size());
    for(auto const& key_value : _data_pset) res.push_back(key_value.first);
    return res;
  }
  bool PSet::contains_value (const std::string& key) const
  {
    return (_data_value.find(key) != _data_value.end());
  }
  bool PSet::contains_pset (const std::string& key) const
  {
    return (_data_pset.find(key) != _data_pset.end());
  }

  void PSet::strip(std::string& str, const std::string& key)
  {
    if(str.find(key) != 0) return;
    while(str.find(key) == 0)
      str = str.substr(key.size(),str.size());
  }
  
  void PSet::rstrip(std::string& str, const std::string& key)
  {
    size_t index = str.rfind(key);
    if(index >= str.size()) return;
    while(key.size() == (str.size() - index)) {
      str = str.substr(0,index);
      index = str.rfind(key);
      if(index >= str.size()) break;
    }
  }

  void PSet::trim_space(std::string& txt){
    strip  ( txt, " "  );
    strip  ( txt, "\t" );
    strip  ( txt, " "  );
    rstrip ( txt, " "  );
    rstrip ( txt, "\t" );
    rstrip ( txt, " "  );
  }
  
  void PSet::no_space(std::string& txt){
    trim_space(txt);
    if(txt.find(" ") < txt.size()) {
      std::stringstream ss;
      ss << " Processing: " << txt.c_str() << " ... Space not allowed!";
      LARCV_CRITICAL() << ss.str() << std::endl;
      throw larbys(ss.str());
    }
    if(txt.find("\t") < txt.size()){
      LARCV_CRITICAL() << "Tab not allowed!" << std::endl;
      throw larbys("Tab not allowed!");
    }
  }

  std::pair<PSet::KeyChar_t,size_t> PSet::search(const std::string& txt, const size_t start) const
  {
    std::pair<KeyChar_t,size_t> res(kNone,size_t(-1));
    size_t index = 0;

    //
    // kParamDef
    //
    index = txt.find(":",start);
    if(index != std::string::npos && index < res.second) {
      res.first  = kParamDef;
      res.second = index;
    }
    index = txt.find("{",start);
    if(index != std::string::npos && index < res.second) {
      res.first  = kBlockStart;
      res.second = index;
    }
    index = txt.find("}",start);
    if(index != std::string::npos && index < res.second) {
      res.first  = kBlockEnd;
      res.second = index;
    }
    /*
    index = txt.find("\"",start);    
    if(index != std::string::npos && index < res.second) {
      res.first  = kString;
      res.second = index;
    }
    */
    return res;
  }

  std::string & PSet::operator[] (std::string key){
    if ( ! this->contains_value(key)){
      this->add_value(key, "");
    }
    return _data_value[key];
  }


  void PSet::add_value(std::string key,
                       std::string value)
  {
    //std::cout<<"  "<<key<<" => "<<value<<std::endl;
    if( _data_value.find(key) != _data_value.end() ||
        _data_pset.find(key)  != _data_pset.end() ) {
      std::string msg;
      msg = " Duplicate key: \"" + key + "\"";
      LARCV_CRITICAL() << msg << std::endl;
      throw larbys(msg.c_str());
    }
    no_space(key);
    if(key.empty()) {
      LARCV_CRITICAL() << "Empty key cannot be registered!" << std::endl;
      throw larbys("Empty key cannot be registered!");
    }
    //std::cout<<"value: @"<<value<<"@"<<std::endl;
    trim_space(value);
    //if(value.empty()) throw larbys("Empty value cannot be registered!");
    _data_value[key]=value;
  }

  void PSet::add_pset(const PSet& p)
  {
    if( _data_value.find(p.name()) != _data_value.end() ||
        _data_pset.find(p.name())  != _data_pset.end() ) {
      std::string msg;
      msg = " Duplicate key: \"" + p.name() + "\"";
      LARCV_CRITICAL() << msg << std::endl;
      throw larbys(msg.c_str());
    }    
    _data_pset.insert(std::make_pair(p.name(),p));
  }
  
  void PSet::add_pset(std::string key,
                      std::string value)
  {
    if( _data_value.find(key) != _data_value.end() ||
        _data_pset.find(key)  != _data_pset.end() ) {
      std::string msg;
      msg = " Duplicate key: \"" + key + "\"";
      LARCV_CRITICAL() << msg << std::endl;
      throw larbys(msg.c_str());
    }
    no_space(key);
    if(key.empty()) {
      LARCV_CRITICAL() << "Empty key cannot be registered!" << std::endl;
      throw larbys("Empty key cannot be registered!");
    }
    strip(value," ");
    rstrip(value," ");
    _data_pset.emplace(key,PSet(key,value));
  }
  

  void PSet::update(std::string key, std::string value){
    std::cout << "Udpate called" << std::endl;
    if ( ! this->contains_value(key)){
      this->add_value(key, value);
    }
    else{
      //std::cout<<"value: @"<<value<<"@"<<std::endl;
      trim_space(value);
      //if(value.empty()) throw larbys("Empty value cannot be registered!");
      _data_value[key]=value;
    }
  }

  void PSet::add(const std::string& contents)
  {
    if(contents.size()<1) return;
    size_t index=0;
    while(contents.find(" ",index) == index)
      index +=1;
    if(index >= contents.size()) return;
    
    size_t end_index=contents.size()-1;
    while(contents.rfind(" ",end_index) == end_index)
      end_index -= 1;

    if(end_index <= index || end_index > contents.size()) return;

    std::string key,value,tmp;
    KeyChar_t last_mark=kNone;
    while(index <= end_index) {
      
      auto next_marker = this->search(contents,index);
      if(next_marker.first == kString) {

        //std::cout<<"String found in here: "<<contents.substr(index,(next_marker.second-index))<<std::endl;

        while(next_marker.second < end_index+1) {

          next_marker = this->search(contents,next_marker.second+1);

          if(next_marker.first == kString) {
            next_marker = this->search(contents,next_marker.second+1);
            break;
          }
        }
        //std::cout<<"String found in here: "<<contents.substr(index,next_marker.second-index)<<std::endl;
      }
      if(next_marker.second > end_index) break;
      if(next_marker.first == kNone) break;
      /*
      std::cout<<"index: "<<index<<std::endl;
      std::cout<<"marker: "<<next_marker.second<<std::endl;
      std::cout<<"type  : "<<next_marker.first<<std::endl;
      std::cout<<"last  : "<<last_mark<<std::endl;
      std::cout<<"Inspecting: "<<"\""<<contents.substr(index,(next_marker.second-index))<<"\""<<std::endl;
      */
      
      if(next_marker.first == kParamDef) {
        if(last_mark ==  kNone || last_mark == kBlockEnd){
          key = contents.substr(index,(next_marker.second-index));
          no_space(key);
        }
        else if(last_mark == kParamDef || last_mark == kString) {
          tmp = contents.substr(index,(next_marker.second-index));
          //std::cout<<"Inspecting: \""<<tmp<<"\"" <<std::endl;
          strip(tmp," ");
          rstrip(tmp," ");
          size_t sep_index = tmp.rfind(" ");
          if(sep_index >= tmp.size()){
            LARCV_CRITICAL() << "Invalid format (key:value)" << std::endl;
            throw larbys("Invalid format (key:value)");
          }

          value = tmp.substr(0,sep_index);
          // complete pair
          this->add_value(key,value);
          //std::cout<<"Found value: "<<value<<std::endl;          
          key = value = "";
          key = tmp.substr(sep_index+1,(tmp.size()-sep_index-1));
          no_space(key);
        }
        
      }else if(next_marker.first == kBlockEnd){
        LARCV_CRITICAL() << "Block end logic error!" << std::endl;
        throw larbys("Block end logic error!");
      }

      else if(next_marker.first == kBlockStart){
        //std::cout<<"Block start!"<<std::endl;
        if(last_mark != kParamDef){
          LARCV_CRITICAL() << "Invalid paramter set start!" << std::endl;
          throw larbys("Invalid paramter set start!");
        }
        
        // fast forward till this block ends
        int start_ctr = 1;
        index = next_marker.second + 1;
        while(start_ctr && next_marker.second <= end_index) {
          
          next_marker = this->search(contents,next_marker.second+1);

          if(next_marker.first == kString) {

            while(next_marker.second < end_index+1) {

              next_marker = this->search(contents,next_marker.second+1);

              auto tmp_next_marker = this->search(contents,next_marker.second+1);

              if(next_marker.first == kString && tmp_next_marker.second != kString) {

                next_marker = tmp_next_marker;
                break;
              }

            }
            //std::cout<<"Found string :"<<contents.substr(index,next_marker.second)<<std::endl;
          }

          switch(next_marker.first){
          case kBlockStart:
            start_ctr +=1;
            break;
          case kBlockEnd:
            start_ctr -=1;
            break;
          default:
            break;
          }
        }
        if(start_ctr) {
          std::string msg;
          msg = "Invalid block:\n" + contents.substr(index,next_marker.second-index) + "\n";
          LARCV_CRITICAL() << msg << std::endl;
          throw larbys(msg.c_str());
        }
        value = contents.substr(index,next_marker.second-index);
        //std::cout<<"PSET!\n"<<value<<"\n"<<std::endl;
        // complete key/value
        this->add_pset(key,value);

        key="";
        value="";
      }
      else {
        LARCV_CRITICAL() << "Unknown error!" << std::endl;
        throw larbys("Unknown error!");
      }
      index = next_marker.second+1;
      last_mark = next_marker.first;
    }

    if(index <= end_index) {
      
      if(!value.empty()) {
        LARCV_CRITICAL() << "Non-empty value @ process-end!" << std::endl;
        throw larbys("Non-empty value @ process-end!");
      }
      if(key.empty()) {
        LARCV_CRITICAL() << "Empty key @ process-end!" << std::endl;
        throw larbys("Empty key @ process-end!");
      }
      
      tmp = contents.substr(index+1,end_index-index);
      //no_space(tmp);
      trim_space(tmp);
      if(tmp.empty()) {
        LARCV_CRITICAL() << "Empty value @ end!" << std::endl;
        throw larbys("Empty value @ end!");
      }
      value = tmp;
      this->add_value(key,value);

      key = "";
      value = "";
    }
    
    if(!key.empty() || !value.empty())
      throw larbys("Unpaired key:value!");

  }
  
  std::string PSet::dump(size_t indent_size) const
  {
    
    std::string res,in_indent,out_indent;
    for(size_t i=0; i<indent_size; ++i) out_indent += " ";
    res = out_indent + _name + " : {\n";
    in_indent = out_indent + "  ";
    for(auto const& key_value : _data_value)

      res += in_indent + key_value.first + " : " + key_value.second + "\n";

    for(auto const& key_value : _data_pset)

      res += key_value.second.dump(in_indent.size()) + "\n";

    res += out_indent + "}\n";
    return res;
  }

  std::string PSet::data_string() const
  {

    std::string res;
    res = _name + ": {";
    for(auto const& key_value : _data_value)

      res += key_value.first + ": " + key_value.second + " ";

    for(auto const& key_value : _data_pset)

      res += key_value.second.data_string();

    res += "} ";
    return res;
  }

  const PSet& PSet::get_pset(const std::string& key) const
  {
    auto iter = _data_pset.find(key);
    if( iter == _data_pset.end() ) {
      std::string msg;
      msg = "Key does not exist: \"" + key + "\"";
      LARCV_CRITICAL() << msg << std::endl;
      throw larbys(msg);
    }
    return (*iter).second;

  }

  template<>
  PSet PSet::get<larcv3::PSet>(const std::string& key) const
  { return this->get_pset(key); }

}

#include <pybind11/operators.h>


void init_PSet(pybind11::module m){
  using Class = larcv3::PSet;
  pybind11::class_<Class> pset(m, "PSet");
  pset.def(pybind11::init<const std::string &, const std::string &>(),
            pybind11::arg("name")="",
            pybind11::arg("data")="");


  pset.def(pybind11::init<const Class &>(),
            pybind11::arg("orig"));


  pset.def(pybind11::self == pybind11::self);
  pset.def(pybind11::self != pybind11::self);
  pset.def("__getitem__",  &Class::operator[], pybind11::is_operator());
  pset.def("__setitem__",  &Class::update);


  pset.def("rename",         &Class::rename);
  pset.def("name",           &Class::name);
  pset.def("clear",          &Class::clear);
  pset.def("add",            &Class::add);
  // pset.def("add_pset",    &Class::add_pset);
  // pset.def("add_pset",    &Class::add_pset);
  pset.def("add_pset",       (void (Class::*)(const Class& p))(&Class::add_pset));
  pset.def("add_value",      &Class::add_value);
  pset.def("dump",           &Class::dump, pybind11::arg("indent_size")=0);
  pset.def("__repr__",       &Class::dump, pybind11::arg("indent_size")=0);
  pset.def("data_string",    &Class::data_string);
  pset.def("get_pset",       &Class::get_pset);

  pset.def("size",           &Class::size);
  pset.def("keys",           &Class::keys);
  pset.def("value_keys",     &Class::value_keys);
  pset.def("pset_keys",      &Class::pset_keys);
  pset.def("contains_value", &Class::contains_value);
  pset.def("contains_pset",  &Class::contains_pset);
    
}




#endif
