/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding cogoright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a cogo of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <algorithm>
#include "t_generator.h"
#include "platform.h"
using namespace std;


/**
 * Go code generator.
 *
 */
class t_go_generator : public t_generator {
 public:
  t_go_generator(
      t_program* program,
      const std::map<std::string, std::string>& parsed_options,
      const std::string& option_string)
    : t_generator(program)
  {
    std::map<std::string, std::string>::const_iterator iter;
    out_dir_base_ = "gen-go";
  }

  /**
   * Init and close methods
   */

  void init_generator();
  void close_generator();

  /**
   * Program-level generation functions
   */

  void generate_typedef  (t_typedef*  ttypedef);
  void generate_enum     (t_enum*     tenum);
  void generate_const    (t_const*    tconst);
  void generate_struct   (t_struct*   tstruct);
  void generate_xception (t_struct*   txception);
  void generate_service  (t_service*  tservice);

  std::string render_const_value(t_type* type, t_const_value* value);

  /**
   * Struct generation code
   */

  void generate_go_struct(t_struct* tstruct, bool is_exception);
  void generate_go_struct_definition(std::ofstream& out, t_struct* tstruct, bool is_xception=false, bool is_result=false);
  void generate_go_struct_reader(std::ofstream& out, t_struct* tstruct, const string& tstruct_name);
  void generate_go_struct_writer(std::ofstream& out, t_struct* tstruct, const string& tstruct_name);
  void generate_go_function_helpers(t_function* tfunction);

  /**
   * Service-level generation functions
   */

  void generate_service_helpers   (t_service*  tservice);
  void generate_service_interface (t_service* tservice);
  void generate_service_client    (t_service* tservice);
  void generate_service_remote    (t_service* tservice);
  void generate_service_server    (t_service* tservice);
  void generate_process_function  (t_service* tservice, t_function* tfunction);

  /**
   * Serialization constructs
   */

  void generate_deserialize_field        (std::ofstream &out,
                                          t_field*    tfield,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err",
                                          bool inclass=false,
                                          bool coerceData=false);

  void generate_deserialize_struct       (std::ofstream &out,
                                          t_struct*   tstruct,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_deserialize_container    (std::ofstream &out,
                                          t_type*     ttype,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_deserialize_set_element  (std::ofstream &out,
                                          t_set*      tset,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_deserialize_map_element  (std::ofstream &out,
                                          t_map*      tmap,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_deserialize_list_element (std::ofstream &out,
                                          t_list*     tlist,
                                          bool        declare,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_serialize_field          (std::ofstream &out,
                                          t_field*    tfield,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_serialize_struct         (std::ofstream &out,
                                          t_struct*   tstruct,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_serialize_container      (std::ofstream &out,
                                          t_type*     ttype,
                                          std::string prefix="",
                                          std::string err="err");

  void generate_serialize_map_element    (std::ofstream &out,
                                          t_map*      tmap,
                                          std::string kiter,
                                          std::string viter,
                                          std::string err="err");

  void generate_serialize_set_element    (std::ofstream &out,
                                          t_set*      tmap,
                                          std::string iter,
                                          std::string err="err");

  void generate_serialize_list_element   (std::ofstream &out,
                                          t_list*     tlist,
                                          std::string iter,
                                          std::string err="err");

  void generate_go_docstring             (std::ofstream& out,
                                          t_struct* tstruct);

  void generate_go_docstring             (std::ofstream& out,
                                          t_function* tfunction);

  void generate_go_docstring             (std::ofstream& out,
                                          t_doc*    tdoc,
                                          t_struct* tstruct,
                                          const char* subheader);

  void generate_go_docstring             (std::ofstream& out,
                                          t_doc* tdoc);

  /**
   * Helper rendering functions
   */

  std::string go_autogen_comment();
  std::string go_package();
  std::string go_imports();
  std::string render_includes();
  std::string render_fastbinary_includes();
  std::string declare_argument(t_field* tfield);
  std::string render_field_default_value(t_field* tfield);
  std::string type_name(t_type* ttype);
  std::string function_signature(t_function* tfunction, std::string prefix="");
  std::string function_signature_if(t_function* tfunction, std::string prefix="", bool addOsError=false);
  std::string argument_list(t_struct* tstruct);
  std::string type_to_enum(t_type* ttype);
  std::string type_to_go_type(t_type* ttype);
  std::string type_to_spec_args(t_type* ttype);

  static std::string get_real_go_module(const t_program* program) {
    std::string real_module = program->get_namespace("go");
    if (real_module.empty()) {
      return program->get_name();
    }
    return real_module;
  }

 private:
  
  /**
   * File streams
   */

  std::ofstream f_types_;
  //std::ofstream f_consts_;
  std::ofstream f_service_;

  std::string package_name_;
  std::string package_dir_;
  
  static std::string publicize(const std::string& value);
  static std::string privatize(const std::string& value);
  static bool can_be_nil(t_type* value);

};


std::string t_go_generator::publicize(const std::string& value) {
  if(value.size() <= 0) return value;
  std::string value2(value);
  if(!isupper(value2[0]))
    value2[0] = toupper(value2[0]);
  // as long as we are changing things, let's change _ followed by lowercase to capital
  for(string::size_type i=1; i<value2.size()-1; ++i) {
    if(value2[i] == '_' && isalpha(value2[i+1])) {
      value2.replace(i, 2, 1, toupper(value2[i+1]));
    }
  }
  return value2;
}

std::string t_go_generator::privatize(const std::string& value) {
  if(value.size() <= 0) return value;
  std::string value2(value);
  if(!islower(value2[0])) {
    value2[0] = tolower(value2[0]);
  }
  // as long as we are changing things, let's change _ followed by lowercase to capital
  for(string::size_type i=1; i<value2.size()-1; ++i) {
    if(value2[i] == '_' && isalpha(value2[i+1])) {
      value2.replace(i, 2, 1, toupper(value2[i+1]));
    }
  }
  return value2;
}

/**
 * Prepares for file generation by opening up the necessary file output
 * streams.
 *
 * @param tprogram The program to generate
 */
void t_go_generator::init_generator() {
  // Make output directory
  string module = get_real_go_module(program_);
  string target = module;
  package_dir_ = get_out_dir();
  while (true) {
    // TODO: Do better error checking here.
    MKDIR(package_dir_.c_str());
    if (module.empty()) {
      break;
    }
    string::size_type pos = module.find('.');
    if (pos == string::npos) {
      package_dir_ += "/";
      package_dir_ += module;
      package_name_ = module;
      module.clear();
    } else {
      package_dir_ += "/";
      package_dir_ += module.substr(0, pos);
      module.erase(0, pos+1);
    }
  }
  string::size_type loc;
  while((loc = target.find(".")) != string::npos) {
    target.replace(loc, 1, 1, '/');
  }
  
  // Make output file
  string f_types_name = package_dir_+"/"+"ttypes.go";
  f_types_.open(f_types_name.c_str());

  //string f_consts_name = package_dir_+"/"+"constants.go";
  //f_consts_.open(f_consts_name.c_str());

  vector<t_service*> services = program_->get_services();
  vector<t_service*>::iterator sv_iter;
  string f_init_name = package_dir_+"/Makefile";
  ofstream f_init;
  f_init.open(f_init_name.c_str());
  f_init  <<
    "all: install" << endl << endl <<
    "TARG=" << target << "" << endl << endl <<
    "DIRS=\\" << endl;
  for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
    f_init << "  " << (*sv_iter)->get_name() << "\\" << endl;
  }
  f_init << endl <<
    "GOFILES=\\" << endl <<
    "  ttypes.go\\" << endl;
  //  "   constants.go\\" << endl;
  for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
    f_init << "  " << (*sv_iter)->get_name() << ".go\\" << endl;
  }
  f_init << "" << endl << endl <<
    "include $(GOROOT)/src/Make.$(GOARCH)" << endl <<
    "include $(GOROOT)/src/Make.pkg" << endl << endl;
  f_init.close();

  for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
    string service_dir = package_dir_+"/"+(*sv_iter)->get_name();
    mkdir(service_dir.c_str(), 0755);
    string f_init_name = service_dir+"/Makefile";
    ofstream f_init;
    f_init.open(f_init_name.c_str());
    f_init  <<
      "all: install" << endl << endl <<
      "TARG=" << target << "Remote" << endl << endl <<
      "DIRS=\\" << endl << endl <<
      "GOFILES=\\" << endl <<
      "  " << (*sv_iter)->get_name() << "-remote.go\\" << endl << endl <<
      "include $(GOROOT)/src/Make.$(GOARCH)" << endl <<
      "include $(GOROOT)/src/Make.pkg" << endl << endl;
    f_init.close();
  }

  // Print header
  f_types_ <<
    go_autogen_comment() <<
    go_package() << 
    go_imports() <<
    render_includes() <<
    render_fastbinary_includes() << endl << endl;

  //f_consts_ <<
  //  go_autogen_comment() <<
  //  go_package() << 
  //  go_imports() <<
  //  endl;
}

/**
 * Renders all the imports necessary for including another Thrift program
 */
string t_go_generator::render_includes() {
  const vector<t_program*>& includes = program_->get_includes();
  string result = "";
  for (size_t i = 0; i < includes.size(); ++i) {
    result += "import \"" + get_real_go_module(includes[i]) + "\";\n";
  }
  if (includes.size() > 0) {
    result += "\n";
  }
  return result;
}

/**
 * Renders all the imports necessary to use the accelerated TBinaryProtocol
 */
string t_go_generator::render_fastbinary_includes() {
  return "";
}

/**
 * Autogen'd comment
 */
string t_go_generator::go_autogen_comment() {
  return
    "/* Autogenerated by Thrift\n"
    " *\n"
    " * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING\n"
    " */\n";
}

/**
 * Prints standard thrift package
 */
string t_go_generator::go_package() {
  return
    string("package ") + package_name_ + ";\n\n";
}

/**
 * Prints standard thrift imports
 */
string t_go_generator::go_imports() {
  return
    string("import (\n"
//           "        \"thrift\";\n"
           "        \"thrift/protocol\";\n"
//           "        \"thrift/base\";\n"
//           "        \"strings\";\n"
           "        \"fmt\";\n"
           ")\n\n");
}

/**
 * Closes the type files
 */
void t_go_generator::close_generator() {
  // Close types file
  f_types_.close();
  //f_consts_.close();
}

/**
 * Generates a typedef. This is not done in go, types are all implicit.
 *
 * @param ttypedef The type definition
 */
void t_go_generator::generate_typedef(t_typedef* ttypedef) {
  
  generate_go_docstring(f_types_, ttypedef);
  f_types_ <<
    "type " << ttypedef->get_symbolic() << " " << type_to_go_type(ttypedef->get_type()) << endl << endl;
}

/**
 * Generates code for an enumerated type. Done using a class to scope
 * the values.
 *
 * @param tenum The enumeration
 */
void t_go_generator::generate_enum(t_enum* tenum) {
  std::ostringstream to_string_mapping, from_string_mapping;
  std::string tenum_name(publicize(tenum->get_name()));

  generate_go_docstring(f_types_, tenum);
  f_types_ <<
    "type " << tenum_name << " int;" << endl <<
    "const (" << endl;
  
  to_string_mapping <<
    indent() << "func (p " << tenum_name << ") String() string {" << endl <<
    indent() << "  switch p {" << endl;
  from_string_mapping <<
    indent() << "func From" << tenum_name << "String(s string) " << tenum_name << " {" << endl <<
    indent() << "  switch s {" << endl;

  vector<t_enum_value*> constants = tenum->get_constants();
  vector<t_enum_value*>::iterator c_iter;
  int value = -1;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    if ((*c_iter)->has_value()) {
      value = (*c_iter)->get_value();
    } else {
      ++value;
    }
    string iter_std_name(escape_string((*c_iter)->get_name()));
    string iter_name((*c_iter)->get_name());
    f_types_ <<
      indent() << "  " << iter_name << ' ' << tenum_name << " = " << value << ";" << endl;
    
    // Dictionaries to/from string names of enums
    to_string_mapping <<
      indent() << "  case " << iter_name << ": return \"" << iter_std_name << "\";" << endl;
    if(iter_std_name != escape_string(iter_name)) {
      from_string_mapping <<
        indent() << "  case \"" << iter_std_name << "\", \"" << escape_string(iter_name) << "\": return " << iter_name << ";" << endl;
    } else {
      from_string_mapping <<
        indent() << "  case \"" << iter_std_name << "\": return " << iter_name << ";" << endl;
    }
  }
  to_string_mapping <<
    indent() << "  }" << endl <<
    indent() << "  return \"\";" << endl <<
    indent() << "}" << endl;
  from_string_mapping <<
    indent() << "  }" << endl <<
    indent() << "  return " << tenum_name << "(-10000);" << endl <<
    indent() << "}" << endl;

  f_types_ <<
    indent() << ")" << endl <<
    to_string_mapping.str() << endl << from_string_mapping.str() << endl <<
    indent() << "func (p " << tenum_name << ") Value() int {" << endl <<
    indent() << "  return int(p);" << endl <<
    indent() << "}" << endl << endl <<
    indent() << "func (p " << tenum_name << ") IsEnum() bool {" << endl <<
    indent() << "  return true;" << endl <<
    indent() << "}" << endl << endl;
}

/**
 * Generate a constant value
 */
void t_go_generator::generate_const(t_const* tconst) {
  t_type* type = tconst->get_type();
  string name = publicize(tconst->get_name());
  t_const_value* value = tconst->get_value();

  indent(f_types_) << "const " << name << " = " << render_const_value(type, value) << ";" << endl;
}

/**
 * Prints the value of a constant with the given type. Note that type checking
 * is NOT performed in this function as it is always run beforehand using the
 * validate_types method in main.cc
 */
string t_go_generator::render_const_value(t_type* type, t_const_value* value) {
  type = get_true_type(type);
  std::ostringstream out;

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_STRING:
      out << '"' << get_escaped_string(value) << '"';
      break;
    case t_base_type::TYPE_BOOL:
      out << (value->get_integer() > 0 ? "true" : "false");
      break;
    case t_base_type::TYPE_BYTE:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
      out << value->get_integer();
      break;
    case t_base_type::TYPE_DOUBLE:
      if (value->get_type() == t_const_value::CV_INTEGER) {
        out << value->get_integer();
      } else {
        out << value->get_double();
      }
      break;
    default:
      throw "compiler error: no const of base type " + t_base_type::t_base_name(tbase);
    }
  } else if (type->is_enum()) {
    indent(out) << value->get_integer();
  } else if (type->is_struct() || type->is_xception()) {
    out << type->get_name() << "(**{" << endl;
    indent_up();
    const vector<t_field*>& fields = ((t_struct*)type)->get_members();
    vector<t_field*>::const_iterator f_iter;
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      t_type* field_type = NULL;
      for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
        if ((*f_iter)->get_name() == v_iter->first->get_string()) {
          field_type = (*f_iter)->get_type();
        }
      }
      if (field_type == NULL) {
        throw "type error: " + type->get_name() + " has no field " + v_iter->first->get_string();
      }
      out << indent();
      out << render_const_value(g_type_string, v_iter->first);
      out << " : ";
      out << render_const_value(field_type, v_iter->second);
      out << "," << endl;
    }
    indent_down();
    indent(out) << "})";
  } else if (type->is_map()) {
    t_type* ktype = ((t_map*)type)->get_key_type();
    t_type* vtype = ((t_map*)type)->get_val_type();
    out << "{" << endl;
    indent_up();
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      out << indent();
      out << render_const_value(ktype, v_iter->first);
      out << " : ";
      out << render_const_value(vtype, v_iter->second);
      out << "," << endl;
    }
    indent_down();
    indent(out) << "}";
  } else if (type->is_list() || type->is_set()) {
    t_type* etype;
    if (type->is_list()) {
      etype = ((t_list*)type)->get_elem_type();
    } else {
      etype = ((t_set*)type)->get_elem_type();
    }
    if (type->is_set()) {
      out << "set(";
    }
    out << "[" << endl;
    indent_up();
    const vector<t_const_value*>& val = value->get_list();
    vector<t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      out << indent();
      out << render_const_value(etype, *v_iter);
      out << "," << endl;
    }
    indent_down();
    indent(out) << "]";
    if (type->is_set()) {
      out << ")";
    }
  } else {
    throw "CANNOT GENERATE CONSTANT FOR TYPE: " + type->get_name();
  }

  return out.str();
}

/**
 * Generates a go struct
 */
void t_go_generator::generate_struct(t_struct* tstruct) {
  generate_go_struct(tstruct, false);
}

/**
 * Generates a struct definition for a thrift exception. Basically the same
 * as a struct but extends the Exception class.
 *
 * @param txception The struct definition
 */
void t_go_generator::generate_xception(t_struct* txception) {
  generate_go_struct(txception, true);
}

/**
 * Generates a go struct
 */
void t_go_generator::generate_go_struct(t_struct* tstruct,
                                        bool is_exception) {
  generate_go_struct_definition(f_types_, tstruct, is_exception);
}

/**
 * Generates a struct definition for a thrift data type.
 *
 * @param tstruct The struct definition
 */
void t_go_generator::generate_go_struct_definition(ofstream& out,
                                                   t_struct* tstruct,
                                                   bool is_exception,
                                                   bool is_result) {

  const vector<t_field*>& members = tstruct->get_members();
  const vector<t_field*>& sorted_members = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator m_iter;

  generate_go_docstring(out, tstruct);
  std::string tstruct_name(publicize(tstruct->get_name()));
  out << "type " << tstruct_name << " struct {" << endl;

  /*
     Here we generate the structure specification for the fastbinary codec.
     These specifications have the following structure:
     thrift_spec -> tuple of item_spec
     item_spec -> nil | (tag, type_enum, name, spec_args, default)
     tag -> integer
     type_enum -> TType.I32 | TType.STRING | TType.STRUCT | ...
     name -> string_literal
     default -> nil  # Handled by __init__
     spec_args -> nil  # For simple types
                | (type_enum, spec_args)  # Value type for list/set
                | (type_enum, spec_args, type_enum, spec_args)
                  # Key and value for map
                | (class_name, spec_args_ptr) # For struct/exception
     class_name -> identifier  # Basically a pointer to the class
     spec_args_ptr -> expression  # just class_name.spec_args

     TODO(dreiss): Consider making this work for structs with negative tags.
  */

  // TODO(dreiss): Look into generating an empty tuple instead of nil
  // for structures with no members.
  // TODO(dreiss): Test encoding of structs where some inner structs
  // don't have thrift_spec.
  indent_up();
  if (sorted_members.empty() || (sorted_members[0]->get_key() >= 0)) {
    int sorted_keys_pos = 0;
    for (m_iter = sorted_members.begin(); m_iter != sorted_members.end(); ++m_iter) {

      for (; sorted_keys_pos != (*m_iter)->get_key(); sorted_keys_pos++) {
        indent(out) << "// nil # " << sorted_keys_pos << endl;
      }
      t_type* fieldType = (*m_iter)->get_type();
      string goType(type_to_go_type(fieldType));
      indent(out) << publicize((*m_iter)->get_name()) << " " 
                  << goType << "; "
                  << " // " << sorted_keys_pos
                  << endl;

      sorted_keys_pos ++;
    }

  } else {
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      // This fills in default values, as opposed to nulls
      indent(out) << publicize((*m_iter)->get_name()) << " " 
                  << type_to_enum((*m_iter)->get_type()) << ";"
                  << endl;
    }
  }
  indent_down();
  indent(out) << "}" << endl << endl;

  indent(out) << "func New" << tstruct_name << "() *" << tstruct_name << " {" << endl;
  indent_up();
  indent(out) << "return &" << tstruct_name << "{";
  indent_up();
  indent_up();
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    // Initialize fields
    t_type* type = (*m_iter)->get_type();
    if (!type->is_base_type() && !type->is_enum() && (*m_iter)->get_value() != NULL) {
      out << endl <<
             indent() << publicize((*m_iter)->get_name()) << "="
                      << render_field_default_value(*m_iter) << ",";
    }
  }
  indent_down();
  indent_down();
  indent(out) << "};" << endl;
  indent_down();
  indent(out) << "}" << endl << endl;

  generate_go_struct_reader(out, tstruct, tstruct_name);
  generate_go_struct_writer(out, tstruct, tstruct_name);

  // Printing utilities so that on the command line thrift
  // structs look pretty like dictionaries
  out << 
    indent() << "func (p *" << tstruct_name << ") TStructName() string {" << endl <<
    indent() << "  return \"" << escape_string(tstruct_name) << "\";" << endl <<
    indent() << "}" << endl << endl;
  
  out << 
    indent() << "func (p *" << tstruct_name << ") ThriftName() string {" << endl <<
    indent() << "  return \"" << escape_string(tstruct->get_name()) << "\";" << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") String() string {" << endl <<
    indent() << "  return fmt.Sprintf(\"" << escape_string(tstruct_name) << "(%+v)\", *p);" << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") Len() int {" << endl <<
    indent() << "  return " << members.size() << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") Contains(other interface{}) bool {" << endl <<
    indent() << "  return p.TStructFields().Contains(other)" << endl <<
    indent() << "}" << endl << endl;

  // Equality and inequality methods that compare by value
  if(members.size() <= 0) {
    out <<
      indent() << "func (p *" << tstruct_name << ") CompareTo(other interface{}) (int, bool) {" << endl <<
      indent() << "  if other == nil { return 1, true }" << endl <<
      indent() << "  _, ok := other.(" << tstruct_name << ")" << endl <<
      indent() << "  if !ok { return 0, false }" << endl <<
      indent() << "  return 0, true" << endl <<
      indent() << "}" << endl << endl;
  } else {
    out <<
      indent() << "func (p *" << tstruct_name << ") CompareTo(other interface{}) (int, bool) {" << endl <<
      indent() << "  if other == nil { return 1, true }" << endl <<
      indent() << "  data, ok := other.(" << tstruct_name << ")" << endl <<
      indent() << "  if !ok { return 0, false }" << endl;
    indent_up();
    for(m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      t_type* orig_type = (*m_iter)->get_type();
      t_type* type = get_true_type(orig_type);
      string field_name(publicize((*m_iter)->get_name()));
      if(type->is_base_type() || type->is_enum()) {
        if(type->is_bool()) {
          out <<
            indent() << "if cmp := protocol.CompareBool(p." << field_name << ", data." << field_name << "); cmp != 0 { return cmp, true }" << endl;
        } else {
          out <<
            indent() << "if p." << field_name << " != data." << field_name << " {" << endl <<
            indent() << "  if p." << field_name << " < data." << field_name << " { return -1, true }" << endl <<
            indent() << "  return 1, true" << endl <<
            indent() << "}" << endl;
        }
      } else if(type->is_container() || type->is_struct() || type->is_xception()) {
        out <<
          indent() << "if cmp, ok := p." << field_name << ".CompareTo(data." << field_name << "); !ok || cmp != 0 { return cmp, ok}" << endl;
      } else {
        throw "INVALID TYPE IN generate_go_struct_definition: " + type->get_name();
      }
    }
    indent_down();
    out << 
      indent() << "  return 0, true" << endl <<
      indent() << "}" << endl << endl;
  }
  
  // Equality and inequality methods that compare by value
  out <<
    indent() << "func (p *" << tstruct_name << ") Equals(other interface{}) bool {" << endl <<
    indent() << "  cmp, ok := p.CompareTo(other)" << endl <<
    indent() << "  return ok && cmp == 0" << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") GetAttribute(field string) interface{} {" << endl <<
    indent() << "  switch field {" << endl <<
    indent() << "  default: return nil" << endl;
  indent_up();
  for(m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    string thrift_name((*m_iter)->get_name());
    string field_name(publicize(thrift_name));
    out <<
      indent() << "case \"" << escape_string(thrift_name) << "\": return p." << field_name << endl;
  }
  indent_down();
  out <<
    indent() << "  }" << endl <<
    indent() << "  return nil" << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") GetAttributeByFieldId(id int16) interface{} {" << endl <<
    indent() << "  switch id {" << endl <<
    indent() << "  default: return nil" << endl;
  indent_up();
  for(m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    string field_name(publicize((*m_iter)->get_name()));
    out <<
      indent() << "case " << (*m_iter)->get_key() << ": return p." << field_name << endl;
  }
  indent_down();
  out <<
    indent() << "  }" << endl <<
    indent() << "  return nil" << endl <<
    indent() << "}" << endl << endl;
  
  out <<
    indent() << "func (p *" << tstruct_name << ") TStructFields() protocol.TFieldContainer {" << endl <<
    indent() << "  return protocol.NewTFieldContainer([]protocol.TField{" << endl;
  indent_up();
  indent_up();
  for(m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    string thrift_name((*m_iter)->get_name());
    out <<
      indent() << "protocol.NewTField(\"" << escape_string(thrift_name) << "\", " << type_to_enum((*m_iter)->get_type()) << ", "<< (*m_iter)->get_key() << ")," << endl;
  }
  out << 
    indent() << "})" << endl;
  indent_down();
  indent_down();
  out <<
    indent() << "}" << endl << endl;
}

/**
 * Generates the read method for a struct
 */
void t_go_generator::generate_go_struct_reader(ofstream& out,
                                                t_struct* tstruct,
                                                const string& tstruct_name) {
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  string escaped_tstruct_name(escape_string(tstruct->get_name()));

  out <<
    indent() << "func (p *" << tstruct_name << ") Read(iprot protocol.TProtocol) (err protocol.TProtocolException) {" << endl;
  indent_up();

  out <<
    indent() << "_, err = iprot.ReadStructBegin();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionReadStruct(p.ThriftName(), err); }" << endl;

  // Loop over reading in fields
  indent(out) << "for {" << endl;
  indent_up();

  // Read beginning field marker
  out <<
    indent() << "fieldName, fieldTypeId, fieldId, err := iprot.ReadFieldBegin();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionReadField(int(fieldId), fieldName, p.ThriftName(), err); }" << endl;

  // Check for field STOP marker and break
  out <<
    indent() << "if fieldTypeId == protocol.STOP { break; }" << endl;

  // Switch statement on the field we are reading
  bool first = true;

  // Generate deserialization code for known cases
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
      indent(out);
    } else {
      indent(out) << "} else ";
    }
    out << "if fieldId == " << (*f_iter)->get_key() << " || fieldName == \"" << escape_string((*f_iter)->get_name()) << "\" {" << endl;
    indent_up();
    indent(out) << "if fieldTypeId == " << type_to_enum((*f_iter)->get_type()) << " {" << endl;
    indent_up();
    generate_deserialize_field(out, *f_iter, false, "p.");
    indent_down();
    out <<
      indent() << "} else if fieldTypeId == protocol.VOID {" << endl <<
      indent() << "  err = iprot.Skip(fieldTypeId);" << endl <<
      indent() << "  if err != nil { return protocol.NewTProtocolExceptionReadField(int(fieldId), fieldName, p.ThriftName(), err); }" << endl <<
      indent() << "} else {" << endl;
    indent_up();
    generate_deserialize_field(out, *f_iter, false, "p.", "err", false, true);
    indent_down();
    out <<
      indent() << "}" << endl;
    indent_down();
  }

  // In the default case we skip the field
  if (first) {
    out <<
      indent() << "if {" << endl;
  } else {
    out <<
      indent() << "} else {" << endl;
  }
  out <<
    indent() << "  err = iprot.Skip(fieldTypeId);" << endl <<
    indent() << "  if err != nil { return protocol.NewTProtocolExceptionReadField(int(fieldId), fieldName, p.ThriftName(), err); }" << endl <<
    indent() << "}" << endl;

  // Read field end marker
  out <<
    indent() << "err = iprot.ReadFieldEnd();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionReadField(int(fieldId), fieldName, p.ThriftName(), err); }" << endl;

  indent_down();
  out <<
    indent() << "}" << endl <<
    indent() << "err = iprot.ReadStructEnd();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionReadStruct(p.ThriftName(), err); }" << endl <<
    indent() << "return;" << endl;

  indent_down();
  indent(out) << "}" << endl << endl;
}

void t_go_generator::generate_go_struct_writer(ofstream& out,
                                               t_struct* tstruct,
                                               const string& tstruct_name) {
  string name(tstruct->get_name());
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) <<
    "func (p *" << tstruct_name << ") Write(oprot protocol.TProtocol) (err protocol.TProtocolException) {" << endl;
  indent_up();

  out << 
    indent() << "err = oprot.WriteStructBegin(\"" << name << "\")" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionWriteStruct(" <<
                          "p.ThriftName(), err); }" << endl;
  
  string escape_field_name;
  int32_t fieldId = -1;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    escape_field_name = escape_string((*f_iter)->get_name());
    fieldId = (*f_iter)->get_key();
    // Write field header
    if (can_be_nil((*f_iter)->get_type())) {
      indent(out) <<
        "if p." << publicize((*f_iter)->get_name()) << " != nil {" << endl;
    } else {
      indent(out) <<
        "{" << endl;
    }
    indent_up();
    out <<
      indent() << "err = oprot.WriteFieldBegin(\"" <<
                            escape_field_name << "\", " <<
                            type_to_enum((*f_iter)->get_type()) << ", " <<
                            fieldId << ");" << endl <<
      indent() << "if err != nil { return protocol.NewTProtocolExceptionWriteField(" <<
                            fieldId << ", \"" <<
                            escape_field_name << "\", " <<
                            "p.ThriftName(), err); }" << endl;

    // Write field contents
    generate_serialize_field(out, *f_iter, "p.");
    
    // Write field closer
    out <<
      indent() << "err = oprot.WriteFieldEnd();" << endl <<
      indent() << "if err != nil { return protocol.NewTProtocolExceptionWriteField(" <<
                            fieldId << ", \"" <<
                            escape_field_name << "\", " <<
                            "p.ThriftName(), err); }" << endl;

    indent_down();
    indent(out) << "}" << endl;
  }

  // Write the struct map
  out <<
    indent() << "err = oprot.WriteFieldStop();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionWriteField(" <<
                          fieldId << ", \"" <<
                          escape_field_name << "\", " <<
                          "p.ThriftName(), err); }" << endl <<
    indent() << "err = oprot.WriteStructEnd();" << endl <<
    indent() << "if err != nil { return protocol.NewTProtocolExceptionWriteStruct(" <<
                          "p.ThriftName(), err); }" << endl <<
    indent() << "return;" << endl;

  indent_down();
  indent(out) << "}" << endl << endl;
}

/**
 * Generates a thrift service.
 *
 * @param tservice The service definition
 */
void t_go_generator::generate_service(t_service* tservice) {
  string f_service_name = package_dir_+"/"+service_name_+".go";
  f_service_.open(f_service_name.c_str());

  f_service_ <<
    go_autogen_comment() <<
    go_package() <<
    go_imports();

  if (tservice->get_extends() != NULL) {
    f_service_ <<
      "import " << get_real_go_module(tservice->get_extends()->get_program()) <<
      "/" << tservice->get_extends()->get_name() << endl;
  }

  f_service_ <<
                "import (" << endl <<
    indent() << "        \"os\";" << endl <<
    indent() << "        \"thrift\";" << endl <<
    indent() << "        \"thrift/base\";" << endl <<
    indent() << "        \"thrift/transport\";" << endl <<
    indent() << ");" << endl << endl <<
    render_fastbinary_includes();

  f_service_ << endl;

  // Generate the three main parts of the service (well, two for now in PHP)
  generate_service_interface(tservice);
  generate_service_client(tservice);
  generate_service_server(tservice);
  generate_service_helpers(tservice);
  generate_service_remote(tservice);

  // Close service file
  f_service_ << endl;
  f_service_.close();
}

/**
 * Generates helper functions for a service.
 *
 * @param tservice The service to generate a header definition for
 */
void t_go_generator::generate_service_helpers(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  f_service_ <<
    "// HELPER FUNCTIONS AND STRUCTURES" << endl << endl;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* ts = (*f_iter)->get_arglist();
    generate_go_struct_definition(f_service_, ts, false);
    generate_go_function_helpers(*f_iter);
  }
}

/**
 * Generates a struct and helpers for a function.
 *
 * @param tfunction The function
 */
void t_go_generator::generate_go_function_helpers(t_function* tfunction) {
  if (true || !tfunction->is_oneway()) {
    t_struct result(program_, tfunction->get_name() + "_result");
    t_field success(tfunction->get_returntype(), "success", 0);
    if (!tfunction->get_returntype()->is_void()) {
      result.append(&success);
    }

    t_struct* xs = tfunction->get_xceptions();
    const vector<t_field*>& fields = xs->get_members();
    vector<t_field*>::const_iterator f_iter;
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      result.append(*f_iter);
    }
    generate_go_struct_definition(f_service_, &result, false, true);
  }
}

/**
 * Generates a service interface definition.
 *
 * @param tservice The service to generate a header definition for
 */
void t_go_generator::generate_service_interface(t_service* tservice) {
  string extends = "";
  string extends_if = "";
  string serviceName(publicize(tservice->get_name()));
  string interfaceName = "I" + serviceName;
  if (tservice->get_extends() != NULL) {
    extends = publicize(type_name(tservice->get_extends()));
    extends_if = "\n" + indent() + "I" + extends + ";\n";
  }

  f_service_ <<
    indent() << "type " << interfaceName << " interface {" << extends_if;
  indent_up();
  generate_go_docstring(f_service_, tservice);
  vector<t_function*> functions = tservice->get_functions();
  if (!functions.empty()) {
    f_service_ << endl;
    vector<t_function*>::iterator f_iter;
    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      generate_go_docstring(f_service_, (*f_iter));
      f_service_ <<
        indent() << function_signature_if(*f_iter, "", true) << ";" << endl;
    }
  }

  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;
}

/**
 * Generates a service client definition.
 *
 * @param tservice The service to generate a server for.
 */
void t_go_generator::generate_service_client(t_service* tservice) {
  string extends = "";
  string extends_client = "";
  string serviceName(publicize(tservice->get_name()));
  if (tservice->get_extends() != NULL) {
    extends = publicize(type_name(tservice->get_extends()));
    extends_client = extends + "Client";
  }

  generate_go_docstring(f_service_, tservice);
  f_service_ <<
    indent() << "type " << serviceName << "Client struct {" << endl;
  indent_up();
  if(!extends_client.empty()) {
    f_service_ << 
      indent() << extends_client << ";" << endl;
  } else {
    f_service_ <<
      indent() << "transport transport.TTransport;" << endl <<
      indent() << "oprotFactory protocol.TProtocolFactory;" << endl <<
      indent() << "iprot protocol.TProtocol;" << endl <<
      indent() << "oprot protocol.TProtocol;" << endl <<
      indent() << "seqId int32;" << endl /*<<
      indent() << "reqs map[int32]Deferred;" << endl*/;
  }
  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;
  
  // Constructor function
  f_service_ <<
    indent() << "func New" << serviceName << "Factory(t transport.TTransport, f protocol.TProtocolFactory) I" << serviceName << " {" << endl;
  indent_up();
  f_service_ <<
    indent() << "return &" << serviceName << "Client";
  indent_up();
  if (!extends.empty()) {
    f_service_ << 
      "{" << extends_client << ": New" << extends_client << "Factory(t, f);}";
  } else {
    f_service_ << "{transport: t," << endl <<
      indent() << "oprotFactory: f," << endl <<
      indent() << "iprot: nil," << endl <<
      indent() << "oprot: nil," << endl <<
      indent() << "seqId: 0," << endl /*<<
      indent() << "reqs: make(map[int32]Deferred)" << endl*/;
  }
  indent_down();
  f_service_ <<
    indent() << "};" << endl;
  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;
  
  
  // Constructor function
  f_service_ <<
    indent() << "func New" << serviceName << "Protocol(t transport.TTransport, iprot protocol.TProtocol, oprot protocol.TProtocol) I" << serviceName << " {" << endl;
  indent_up();
  f_service_ <<
    indent() << "return &" << serviceName << "Client";
  indent_up();
  if (!extends.empty()) {
    f_service_ << 
      "{" << extends_client << ": New" << extends_client << "Protocol(t, iprot, oprot)};";
  } else {
    f_service_ << "{transport: t," << endl <<
      indent() << "oprotFactory: nil," << endl <<
      indent() << "iprot: iprot," << endl <<
      indent() << "oprot: oprot," << endl <<
      indent() << "seqId: 0," << endl /*<<
      indent() << "reqs: make(map[int32]interface{})" << endl*/;
  }
  indent_down();
  f_service_ <<
    indent() << "};" << endl;
  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;

  // Generate client method implementations
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* arg_struct = (*f_iter)->get_arglist();
    const vector<t_field*>& fields = arg_struct->get_members();
    vector<t_field*>::const_iterator fld_iter;
    string funname = publicize((*f_iter)->get_name());

    // Open function
    generate_go_docstring(f_service_, (*f_iter));
    f_service_ <<
      indent() << "func (p *" << serviceName << "Client) " << function_signature_if(*f_iter, "", true) << " {" << endl;
    indent_up();
    /*
    f_service_ <<
      indent() << "p.seqId += 1;" << endl;
    if (!(*f_iter)->is_oneway()) {
      f_service_ <<
        indent() << "d := defer.Deferred();" << endl <<
        indent() << "p.reqs[p.seqId] = d;" << endl;
    }
    */
    f_service_ <<
      indent() << "err = p.Send" << funname << "(";

    bool first = true;
    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      if (first) {
        first = false;
      } else {
        f_service_ << ", ";
      }
      f_service_ << (*fld_iter)->get_name();
    }
    f_service_ << ");" << endl <<
      indent() << "if err != nil { return; }" << endl;
    

    if (!(*f_iter)->is_oneway()) {
      f_service_ << 
        indent() << "return p.Recv" << funname << "();" << endl;
    } else {
      f_service_ <<
        indent() << "return;" << endl;
    }
    indent_down();
    f_service_ << 
      indent() << "}" << endl << endl <<
      indent() << "func (p *" << serviceName << "Client) Send" << function_signature(*f_iter) << "(err os.Error) {" << endl;
    
    indent_up();
    
    std::string argsname = privatize((*f_iter)->get_name()) + "Args";
    
    // Serialize the request header
    string args(tmp("args"));
    f_service_ <<
      indent() << "oprot := p.oprotFactory.GetProtocol(p.transport);" << endl <<
      indent() << "oprot.WriteMessageBegin(\"" << (*f_iter)->get_name() << "\", protocol.CALL, p.seqId);" << endl <<
      indent() << args << " := New" << publicize(argsname) << "();" << endl;
    
    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      f_service_ <<
        indent() << args << "." << publicize((*fld_iter)->get_name()) << " = " << (*fld_iter)->get_name() << ";" << endl;
    }
    
    // Write to the stream
    f_service_ <<
      indent() << "err = " << args << ".Write(oprot);" << endl <<
      indent() << "oprot.WriteMessageEnd();" << endl <<
      indent() << "oprot.Transport().Flush();" << endl <<
      indent() << "return;" << endl;
    indent_down();
    f_service_ <<
      indent() << "}" << endl << endl;
    
    if (true) { //!(*f_iter)->is_oneway() || true) {}
      std::string resultname = privatize((*f_iter)->get_name()) + "Result";
      // Open function
      f_service_ << endl <<
                    indent() << "func (p *" << serviceName << "Client) Recv" << publicize((*f_iter)->get_name()) <<
                    "() (";
      if(!(*f_iter)->get_returntype()->is_void()) {
        f_service_ <<
          "value " << type_to_go_type((*f_iter)->get_returntype()) << ", ";
      }
      f_service_ << 
        "err os.Error) {" << endl;
      indent_up();

      // TODO(mcslee): Validate message reply here, seq ids etc.
      
      string result(tmp("result"));
      
      f_service_ <<
        indent() << "_, mTypeId, _, err := p.iprot.ReadMessageBegin();" << endl <<
        indent() << "if err != nil { return; }" << endl <<
        indent() << "if mTypeId == protocol.EXCEPTION {" << endl <<
        indent() << "  error := base.NewTApplicationExceptionDefault();" << endl <<
        indent() << "  error, err := error.Read(p.iprot);" << endl <<
        indent() << "  if err != nil { return; }" << endl <<
        indent() << "  err = p.iprot.ReadMessageEnd();" << endl <<
        indent() << "  if err != nil { return; }" << endl <<
        indent() << "  err = error;" << endl <<
        indent() << "  return;" << endl <<
        indent() << "}" << endl <<
        indent() << result << " := New" << publicize(resultname) << "();" << endl <<
        indent() << "err = " << result << ".Read(p.iprot);" << endl <<
        indent() << "p.iprot.ReadMessageEnd();" << endl;

      // Careful, only return _result if not a void function
      if (!(*f_iter)->get_returntype()->is_void()) {
        f_service_ <<
          indent() << "value = " << result << ".Success;" << endl;
      }
      f_service_ <<
        indent() << "return;" << endl;
      
      /*
      t_struct* xs = (*f_iter)->get_xceptions();
      const std::vector<t_field*>& xceptions = xs->get_members();
      vector<t_field*>::const_iterator x_iter;
      for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
        f_service_ <<
          indent() << "if result." << (*x_iter)->get_name() << " != nil {" << endl <<
          indent() << "  return d.errback(result." << (*x_iter)->get_name() << ");" << endl <<
          indent() << "}" << endl;
      }

      // Careful, only return _result if not a void function
      if ((*f_iter)->get_returntype()->is_void()) {
        f_service_ <<
          indent() << "return d.callback(nil)" << endl;
      } else {
        f_service_ <<
          indent() << "return d.errback(base.NewTApplicationException(base.TApplicationException.MISSING_RESULT, \"" << (*f_iter)->get_name() << " failed: unknown result\"));" << endl;
      }
      */
      // Close function
      indent_down();
      f_service_ <<
        indent() << "}" << endl << endl;
    }
  }

  //indent_down();
  f_service_ <<
    endl;
}

/**
 * Generates a command line tool for making remote requests
 *
 * @param tservice The service to generate a remote for.
 */
void t_go_generator::generate_service_remote(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  string f_remote_name = package_dir_+"/"+service_name_+"/"+service_name_+"-remote.go";
  ofstream f_remote;
  f_remote.open(f_remote_name.c_str());

  f_remote <<
    go_autogen_comment() <<
    indent() << "package " << service_name_ << "Remote;" << endl << endl <<
    indent() << "import (" << endl <<
    indent() << "        \"flag\";" << endl <<
    indent() << "        \"fmt\";" << endl <<
    indent() << "        \"http\";" << endl <<
    indent() << "        \"net\";" << endl <<
    indent() << "        \"os\";" << endl <<
    indent() << "        \"strconv\";" << endl <<
    indent() << "        \"thrift/protocol\";" << endl <<
    indent() << "        \"thrift/transport\";" << endl <<
    indent() << "        \"" << package_name_ << "\";" << endl <<
    indent() << ")" << endl <<
    indent() << endl <<
    indent() << "func Usage() {" << endl <<
    indent() << "  fmt.Fprint(os.Stderr, \"Usage of \", os.Args[0], \" [-h host:port] [-u url] [-f[ramed]] function [arg1 [arg2...]]:\\n\");" << endl <<
    indent() << "  flag.PrintDefaults();" << endl <<
    indent() << "  fmt.Fprint(os.Stderr, \"Functions:\\n\");" << endl;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_remote <<
      indent() << "  fmt.Fprint(os.Stderr, \"  " << function_signature_if(*f_iter, "", true) << "\\n\");" << endl;
  }
  f_remote <<
    indent() << "  fmt.Fprint(os.Stderr, \"\\n\");" << endl <<
    indent() << "  os.Exit(0);" << endl <<
    indent() << "}" << endl <<
    indent() << endl <<
    indent() << "func main() {" << endl <<
    indent() << "  flag.Usage = Usage;" << endl <<
    indent() << "  var host string;" << endl <<
    indent() << "  var port int;" << endl <<
    indent() << "  var urlString string;" << endl <<
    indent() << "  var framed bool;" << endl <<
    indent() << "  var useHttp bool;" << endl <<
    indent() << "  var help bool;" << endl <<
    indent() << "  var url http.URL;" << endl <<
    indent() << "  var trans transport.TTransport;" << endl <<
    indent() << "  flag.Usage = Usage;" << endl <<
    indent() << "  flag.StringVar(&host, \"h\", \"localhost\", \"Specify host and port\");" << endl <<
    indent() << "  flag.IntVar(&port, \"p\", 9090, \"Specify port\");" << endl <<
    indent() << "  flag.StringVar(&urlString, \"u\", \"\", \"Specify the url\");" << endl <<
    indent() << "  flag.BoolVar(&framed, \"framed\", false, \"Use framed transport\");" << endl <<
    indent() << "  flag.BoolVar(&useHttp, \"http\", false, \"Use http\");" << endl <<
    indent() << "  flag.BoolVar(&help, \"help\", false, \"See usage string\");" << endl <<
    indent() << "  flag.Parse();" << endl <<
    indent() << "  if help || flag.NArg() == 0 {" << endl <<
    indent() << "    flag.Usage();" << endl <<
    indent() << "  }" << endl <<
    indent() << endl <<
    indent() << "  if len(urlString) > 0 {" << endl <<
    indent() << "    url, err := http.ParseURL(urlString);" << endl <<
    indent() << "    if err != nil {" << endl <<
    indent() << "      fmt.Fprint(os.Stderr, \"Error parsing URL: \", err.String(), \"\\n\");" << endl <<
    indent() << "      flag.Usage();" << endl <<
    indent() << "    }" << endl <<
    indent() << "    host = url.Host" << endl <<
    //indent() << "    if len(url.Port) == 0 { url.Port = \"80\"; }" << endl <<
    //indent() << "    port = int(url.Port)" << endl <<
    indent() << "    useHttp = len(url.Scheme) <= 0 || url.Scheme == \"http\";" << endl <<
    indent() << "  } else if useHttp {" << endl <<
    indent() << "    _, err := http.ParseURL(fmt.Sprint(\"http://\", host, \":\", port));" << endl <<
    indent() << "    if err != nil {" << endl <<
    indent() << "      fmt.Fprint(os.Stderr, \"Error parsing URL: \", err.String(), \"\\n\");" << endl <<
    indent() << "      flag.Usage();" << endl <<
    indent() << "    }" << endl <<
    indent() << "  }" << endl <<
    indent() << endl <<
    indent() << "  cmd := flag.Arg(0);" << endl <<
    indent() << "  var err os.Error;" << endl <<
    indent() << "  if useHttp {" << endl <<
    indent() << "    trans, err = transport.NewTHttpClient(url.Raw);" << endl <<
    indent() << "  } else {" << endl <<
    indent() << "    addr, err := net.ResolveTCPAddr(fmt.Sprint(host, \":\", port));" << endl <<
    indent() << "    if err != nil {" << endl <<
    indent() << "      fmt.Fprint(os.Stderr, \"Error resolving address\", err.String());" << endl <<
    indent() << "      os.Exit(1);" << endl <<
    indent() << "    }" << endl <<
    indent() << "    trans, err = transport.NewTNonblockingSocketAddr(addr);" << endl <<
    indent() << "    if framed {" << endl <<
    indent() << "      trans = transport.NewTFramedTransport(trans);" << endl <<
    indent() << "    }" << endl <<
    indent() << "  }" << endl <<
    indent() << "  if err != nil {" << endl <<
    indent() << "    fmt.Fprint(os.Stderr, \"Error creating transport\", err.String());" << endl <<
    indent() << "    os.Exit(1);" << endl <<
    indent() << "  }" << endl <<
    indent() << "  proto := protocol.NewTBinaryProtocolTransport(trans);" << endl <<
    indent() << "  client := " << package_name_ << ".New" << publicize(service_name_) << "Protocol(trans, proto, proto);" << endl <<
    indent() << "  trans.Open();" << endl <<
    indent() << endl;

  // Generate the dispatch methods
  indent_up();
  f_remote <<
    indent() << "switch cmd {" << endl;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {

    t_struct* arg_struct = (*f_iter)->get_arglist();
    const std::vector<t_field*>& args = arg_struct->get_members();
    vector<t_field*>::const_iterator a_iter;
    int num_args = args.size();
    string pubName(publicize((*f_iter)->get_name()));
    
    f_remote <<
      indent() << "case \"" << escape_string(pubName) << "\":" << endl;
    indent_up();
    f_remote <<
      indent() << "  if flag.NArg() - 1 != " << num_args << " {" << endl <<
      indent() << "    fmt.Fprint(os.Stderr, \"" << escape_string(pubName) << " requires " << num_args << " args\");" << endl <<
      indent() << "    flag.Usage();" << endl <<
      indent() << "  }" << endl;
    for (int i = 0; i < num_args; ++i) {
      if(args[i]->get_type()->is_enum()) {
        f_remote << 
          indent() << "  tmp" << i << ", err := (strconv.Atoi(flag.Arg(" << i << ")));" << endl <<
          indent() << "  if err != nil { Usage(); return; }" << endl <<
          indent() << "  value" << i << " := " << package_name_ << "." << publicize(args[i]->get_type()->get_name()) << "(tmp" << i << ");" << endl;
      } else if(args[i]->get_type()->is_base_type()) {
        t_base_type::t_base e = ((t_base_type*)(args[i]->get_type()))->get_base();
        switch(e) {
          case t_base_type::TYPE_VOID: break;
          case t_base_type::TYPE_STRING:
            f_remote << 
              indent() << "  value" << i << " := flag.Arg(" << i << ");" << endl;
            break;
          case t_base_type::TYPE_BOOL:
            f_remote << 
              indent() << "  value" << i << " := flag.Arg(" << i << ") == \"true\";" << endl;
            break;
          case t_base_type::TYPE_BYTE:
            f_remote << 
              indent() << "  tmp" << i << ", err := (strconv.Atoi(flag.Arg(" << i << ")));" << endl <<
              indent() << "  if err != nil { Usage(); return; }" << endl <<
              indent() << "  value" << i << " := byte(tmp" << i << ");" << endl;
            break;
          case t_base_type::TYPE_I16:
            f_remote << 
              indent() << "  tmp" << i << ", err := (strconv.Atoi(flag.Arg(" << i << ")));" << endl <<
              indent() << "  if err != nil { Usage(); return; }" << endl <<
              indent() << "  value" << i << " := byte(tmp" << i << ");" << endl;
            break;
          case t_base_type::TYPE_I32:
            f_remote << 
              indent() << "  tmp" << i << ", err := (strconv.Atoi(flag.Arg(" << i << ")));" << endl <<
              indent() << "  if err != nil { Usage(); return; }" << endl <<
              indent() << "  value" << i << " := int32(tmp" << i << ");" << endl;
            break;
          case t_base_type::TYPE_I64:
            f_remote <<
              indent() << "  value" << i << ", err := (strconv.Atoi64(flag.Arg(" << i << ")));" << endl <<
              indent() << "  if err != nil { Usage(); return; }" << endl;
            break;
          case t_base_type::TYPE_DOUBLE:
            f_remote <<
              indent() << "  value" << i << ", err := (strconv.Atof64(flag.Arg(" << i << ")));" << endl <<
              indent() << "  if err != nil { Usage(); return; }" << endl;
            break;
          default:
            throw("Invalid base type in generate_service_remote");
        }
        //f_remote << publicize(args[i]->get_name()) << "(strconv.Atoi(flag.Arg(" << i << ")))";
      } else {
        f_remote <<
          indent() << "  value" << i << ", err := eval(flag.Arg(" << i << "));" << endl <<
          indent() << "  if err != nil { Usage(); return; }" << endl;
      }
    }
    f_remote <<
      indent() << "  fmt.Print(client." << pubName << "(";
    bool argFirst = true;
    for (int i = 0; i < num_args; ++i) {
      if (argFirst) {
        argFirst = false;
      } else {
        f_remote << ", ";
      }
      if(args[i]->get_type()->is_enum()) {
        f_remote << "value" << i;
      } else if(args[i]->get_type()->is_base_type()) {
        t_base_type::t_base e = ((t_base_type*)(args[i]->get_type()))->get_base();
        switch(e) {
          case t_base_type::TYPE_VOID: break;
          case t_base_type::TYPE_STRING:
          case t_base_type::TYPE_BOOL:
          case t_base_type::TYPE_BYTE:
          case t_base_type::TYPE_I16:
          case t_base_type::TYPE_I32:
          case t_base_type::TYPE_I64:
          case t_base_type::TYPE_DOUBLE:
            f_remote << "value" << i;
            break;
          default:
            throw("Invalid base type in generate_service_remote");
        }
        //f_remote << publicize(args[i]->get_name()) << "(strconv.Atoi(flag.Arg(" << i << ")))";
      } else {
        f_remote << "value" << i;
      }
    }
    f_remote << "));" << endl;
    indent_down();
  }
  f_remote <<
    indent() << "}" << endl;
  indent_down();
  
  f_remote << 
    indent() << "  trans.Close();" << endl <<
    indent() << "}" << endl;
  

  // Close service file
  f_remote.close();

  // Make file executable, love that bitwise OR action
  chmod(f_remote_name.c_str(),
          S_IRUSR
        | S_IWUSR
        | S_IXUSR
#ifndef MINGW
        | S_IRGRP
        | S_IXGRP
        | S_IROTH
        | S_IXOTH
#endif
  );
}

/**
 * Generates a service server definition.
 *
 * @param tservice The service to generate a server for.
 */
void t_go_generator::generate_service_server(t_service* tservice) {
  // Generate the dispatch methods
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  string extends = "";
  string extends_processor = "";
  string serviceName(publicize(tservice->get_name()));
  if (tservice->get_extends() != NULL) {
    extends = privatize(type_name(tservice->get_extends()));
    extends_processor = extends + "Processor";
  }
  string pServiceName(privatize(serviceName));

  // Generate the header portion
  string self(tmp("self"));
  if(extends_processor.empty()) {
    f_service_ <<
      indent() << "type " << serviceName << "Processor struct {" << endl <<
      indent() << "  handler I" << serviceName << ";" << endl <<
      indent() << "  processMap map[string]base.TProcessor;" << endl <<
      indent() << "}" << endl << endl <<
      indent() << "func (p *" << serviceName << "Processor) Handler() I" << serviceName << " {" << endl <<
      indent() << "  return p.handler;" << endl <<
      indent() << "}" << endl << endl <<
      indent() << "func (p *" << serviceName << "Processor) ProcessMap() map[string]base.TProcessor {" << endl;
    indent_up();
    f_service_ <<
      indent() << "  return p.processMap;" << endl;
    indent_down();
    f_service_ <<
      indent() << "}" << endl << endl <<
      indent() << "func New" << serviceName << "Processor(handler I" << serviceName << ") *" << serviceName << "Processor {" << endl;
    indent_up();
    f_service_ <<
      indent() << self << " := &" << serviceName << "Processor{handler:handler, processMap:make(map[string]base.TProcessor)};" << endl;
    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      string escapedFuncName(escape_string((*f_iter)->get_name()));
      f_service_ <<
        indent() << self << ".processMap[\"" << escapedFuncName << "\"] = &" << pServiceName << "Processor" << publicize((*f_iter)->get_name()) << "{handler:handler};" << endl;
    }
    f_service_ <<
      indent() << "return " << self << ";" << endl;
    indent_down();
    f_service_ <<
      indent() << "}" << endl << endl;
  } else {
    f_service_ <<
      indent() << "type " << serviceName << "Processor struct {" << endl <<
      indent() << "  super " << extends_processor << ";" << endl <<
      indent() << "}" << endl << endl <<
      indent() << "func New" << serviceName << "Processor(handler I" << serviceName << ") *" << serviceName << "Processor {" << endl;
    indent_up();
    f_service_ <<
      indent() << self << " := &" << serviceName << "Processor{super: New" << publicize(extends_processor) << "(handler), processMap:make(map[string]base.TProcessor)};" << endl;
    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      string escapedFuncName(escape_string((*f_iter)->get_name()));
      f_service_ <<
        indent() << self << ".processMap[\"" << escapedFuncName << "\"] = &" << pServiceName << "Processor" << publicize((*f_iter)->get_name()) << "{handler:handler};" << endl;
    }
    f_service_ <<
      indent() << "return " << self << ";" << endl;
    indent_down();
    f_service_ <<
      indent() << "}" << endl << endl;
  }

  // Generate the server implementation
  string x(tmp("x"));
  f_service_ <<
    indent() << "func (p *" << serviceName << "Processor) Process(iprot, oprot protocol.TProtocol, seqid int32) (success bool, err thrift.TException) {" << endl;
  indent_up();

  f_service_ <<
    indent() << "name, _, _, err := iprot.ReadMessageBegin();" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "process, nameFound := p.processMap[name];" << endl <<
    indent() << "if !nameFound || process == nil {" << endl <<
    indent() << "  iprot.Skip(protocol.STRUCT);" << endl <<
    indent() << "  iprot.ReadMessageEnd();" << endl <<
    indent() << "  " << x << " := base.NewTApplicationException(base.UNKNOWN_METHOD, \"Unknown function \" + name);" << endl <<
    indent() << "  oprot.WriteMessageBegin(name, protocol.EXCEPTION, seqid);" << endl <<
    indent() << "  " << x << ".Write(oprot);" << endl <<
    indent() << "  oprot.WriteMessageEnd();" << endl <<
    indent() << "  oprot.Transport().Flush();" << endl <<
    indent() << "  return false, " << x << ";" << endl <<
    indent() << "}" << endl <<
    indent() << "return process.Process(iprot, oprot, seqid);" << endl;

  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;
  
  // Generate the process subfunctions
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    generate_process_function(tservice, *f_iter);
  }

  f_service_ << endl;
}

/**
 * Generates a process function definition.
 *
 * @param tfunction The function to write a dispatcher for
 */
void t_go_generator::generate_process_function(t_service* tservice,
                                               t_function* tfunction) {
  // Open function
  string processorName = privatize(tservice->get_name()) + "Processor" + publicize(tfunction->get_name());
  
  string argsname = publicize(tfunction->get_name()) + "Args";
  string resultname = publicize(tfunction->get_name()) + "Result";
  
  //t_struct* xs = tfunction->get_xceptions();
  //const std::vector<t_field*>& xceptions = xs->get_members();
  vector<t_field*>::const_iterator x_iter;
  
  f_service_ <<
    indent() << "type " << processorName << " struct {" << endl <<
    indent() << "  handler I" << publicize(tservice->get_name()) << ";" << endl <<
    indent() << "};" << endl << endl <<
    indent() << "func (p *" << processorName << ") Process(iprot, oprot protocol.TProtocol, seqid int32) (success bool, err thrift.TException) {" << endl;
  indent_up();
  
  f_service_ <<
    indent() << "args := New" << argsname << "();" << endl <<
    indent() << "err = args.Read(iprot);" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "iprot.ReadMessageEnd();" << endl <<
    indent() << "result := New" << resultname << "();" << endl <<
    indent();
  if (!tfunction->is_oneway() && !tfunction->get_returntype()->is_void()) {
    f_service_ <<
      indent() << "result.Success, ";
  }
  
  // Generate the function call
  t_struct* arg_struct = tfunction->get_arglist();
  const std::vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  f_service_ <<
    "err = p.handler." << publicize(tfunction->get_name()) << "(";
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      f_service_ << ", ";
    }
    f_service_ << "args." << publicize((*f_iter)->get_name());
  }
  f_service_ << ");" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "err = oprot.WriteMessageBegin(\"" << escape_string(tfunction->get_name()) << "\", protocol.REPLY, seqid)" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "err = result.Write(oprot);" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "err = oprot.WriteMessageEnd();" << endl <<
    indent() << "if err != nil { return; }" << endl <<
    indent() << "err = oprot.Transport().Flush();" << endl <<
    indent() << "return true, err;" << endl;
  indent_down();
  f_service_ <<
    indent() << "}" << endl << endl;
  /*
  indent(f_service_) <<
      "func (p *" << publicize(tservice->get_name()) << "Client) WriteResultsSuccess" << publicize(tfunction->get_name()) <<
      "(success bool, result " << publicize(tfunction->get_name()) << "Result, seqid int32, oprot protocol.TProtocol) (err os.Error) {" << endl;
  indent_up();
  f_service_ <<
    indent() << "result.Success = success" << endl <<
    indent() << "oprot.WriteMessageBegin(\"" << escape_string(tfunction->get_name()) << "\", protocol.REPLY, seqid)" << endl <<
    indent() << "result.Write(oprot);" << endl <<
    indent() << "oprot.WriteMessageEnd();" << endl <<
    indent() << "oprot.Transport().Flush();" << endl <<
    indent() << "return;" << endl;
  indent_down();
  f_service_ << 
    indent() << "}" << endl << endl;
  */
  // Try block for a function with exceptions
  /*
  if (!tfunction->is_oneway() && xceptions.size() > 0) {
    indent(f_service_) <<
      "func (p *" << publicize(tservice->get_name()) << "Client) WriteResultsException" << publicize(tfunction->get_name()) <<
      "(error *" << publicize(tfunction->get_name()) << ", result *, seqid, oprot) (err os.Error) {" << endl;
    indent_up();

    // Kinda absurd
    for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
      f_service_ <<
        indent() << "except " << type_name((*x_iter)->get_type()) << ", " << (*x_iter)->get_name() << ":" << endl;
      if (!tfunction->is_oneway()) {
        indent_up();
        f_service_ <<
          indent() << "result." << (*x_iter)->get_name() << " = " << (*x_iter)->get_name() << endl;
        indent_down();
      } else {
        f_service_ <<
          indent() << "pass" << endl;
      }
    }
    f_service_ <<
      indent() << "err = oprot.WriteMessageBegin(\"" << escape_string(tfunction->get_name()) << "\", protocol.REPLY, seqid)" << endl <<
      indent() << "if err != nil { return err; }" << endl <<
      indent() << "err = result.Write(oprot);" << endl <<
      indent() << "if err != nil { return err; }" << endl <<
      indent() << "err = oprot.WriteMessageEnd();" << endl <<
      indent() << "if err != nil { return err; }" << endl <<
      indent() << "err = oprot.Transport().Flush();" << endl <<
      indent() << "if err != nil { return err; }" << endl;
    indent_down();
    f_service_ << "}" << endl << endl;
  }
  */
}

/**
 * Deserializes a field of any type.
 */
void t_go_generator::generate_deserialize_field(ofstream &out,
                                                t_field* tfield,
                                                bool declare,
                                                string prefix,
                                                string err,
                                                bool inclass,
                                                bool coerceData) {
  t_type* orig_type = tfield->get_type();
  t_type* type = get_true_type(orig_type);
  string name(prefix + publicize(tfield->get_name()));

  if (type->is_void()) {
    throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " + name;
  }
  string v = tmp("v");
  string err2 = tmp("err");

  if (type->is_struct() || type->is_xception()) {
    generate_deserialize_struct(out,
                                (t_struct*)type,
                                declare,
                                name,
                                err);
  } else if (type->is_container()) {
    generate_deserialize_container(out, type, declare, name, err);
  } else if (type->is_base_type() || type->is_enum()) {
    indent(out) <<
      v << ", " << err2 << " := iprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " +
          name;
        break;
      case t_base_type::TYPE_STRING:
        out << "ReadString();";
        break;
      case t_base_type::TYPE_BOOL:
        out << "ReadBool();";
        break;
      case t_base_type::TYPE_BYTE:
        out << "ReadByte();";
        break;
      case t_base_type::TYPE_I16:
        out << "ReadI16();";
        break;
      case t_base_type::TYPE_I32:
        out << "ReadI32();";
        break;
      case t_base_type::TYPE_I64:
        out << "ReadI64();";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "ReadDouble();";
        break;
      default:
        throw "compiler error: no Go name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "ReadI32();";
    }
    string structName("\"\"");
    if(!prefix.size() || prefix.find(".") == string::npos) {
      structName = "\"\"";
    } else {
      structName = prefix + "ThriftName()";
    }
    out << endl <<
      indent() << "if " << err2 << " != nil { return protocol.NewTProtocolExceptionReadField(" <<
                           tfield->get_key()  <<
                           ", \"" << escape_string(tfield->get_name()) <<
                           "\", " << structName << ", " << err2 << "); }" << endl;
    if(!prefix.size() || prefix.find(".") == string::npos) {
      if(type->is_enum() || orig_type->is_typedef()) {
        indent(out) << name << " := " << publicize(orig_type->get_name()) << "("<< v << ");" << endl;
      } else {
        indent(out) << name << " := " << v << ";" << endl;
      }
    } else {
      if(type->is_enum() || orig_type->is_typedef()) {
        indent(out) << name << " = " << publicize(orig_type->get_name()) << "("<< v << ");" << endl;
      } else {
        indent(out) << name << " = " << v << ";" << endl;
      }
    }

  } else {
    throw "INVALID TYPE IN generate_deserialize_field '" + type->get_name() + "' for field '" + tfield->get_name() + "'";
  }
}

/**
 * Generates an unserializer for a struct, calling read()
 */
void t_go_generator::generate_deserialize_struct(ofstream &out,
                                                  t_struct* tstruct,
                                                  bool declare,
                                                  string prefix,
                                                  string err) {
  string err2(tmp("err"));
  string eq(" := ");
  if(!declare) {
    eq = " = ";
  }
  out <<
    indent() << prefix << eq << "New" << publicize(type_name(tstruct)) << "();" << endl <<
    indent() << err2 << " := " << prefix << ".Read(iprot)" << endl <<
    indent() << "if " << err2 << " != nil { return protocol.NewTProtocolExceptionReadStruct(\"" <<
                         escape_string(prefix + tstruct->get_name()) << "\", " <<
                         err2 << "); }\n";
}

/**
 * Serialize a container by writing out the header followed by
 * data and then a footer.
 */
void t_go_generator::generate_deserialize_container(ofstream &out,
                                                    t_type* ttype,
                                                    bool   declare,
                                                    string prefix,
                                                    string err) {
  string size = tmp("_size");
  string ktype = tmp("_ktype");
  string vtype = tmp("_vtype");
  string etype = tmp("_etype");

  t_field fsize(g_type_i32, size);
  t_field fktype(g_type_byte, ktype);
  t_field fvtype(g_type_byte, vtype);
  t_field fetype(g_type_byte, etype);
  
  string eq(" = ");
  if(declare)
    eq = " := ";

  // Declare variables, read header
  if (ttype->is_map()) {
    out <<
      indent() << ktype << ", " << vtype << ", " << size << ", " << err << " := iprot.ReadMapBegin();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" <<
                           -1 << ", \"" <<
                           escape_string(prefix) << "\", \"\", " << 
                           err << "); }" << endl <<
      indent() << prefix << eq << "protocol.NewTMap(" << ktype << ", " << vtype << ", " << size << ");" << endl;
  } else if (ttype->is_set()) {
    out <<
      indent() << etype << ", " << size << ", " << err << " := iprot.ReadSetBegin();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" <<
                           -1 << ", \"" <<
                           escape_string(prefix) << "\", \"\", " << 
                           err << "); }" << endl <<
      indent() << prefix << eq << "protocol.NewTSet(" << etype << ", " << size << ");" << endl;
  } else if (ttype->is_list()) {
    out <<
      indent() << etype << ", " << size << ", " << err << " := iprot.ReadListBegin();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" <<
                           -1 << ", \"" <<
                           escape_string(prefix) << "\", \"\", " << 
                           err << "); }" << endl <<
      indent() << prefix << eq << "protocol.NewTList(" << etype << ", " << size << ");" << endl;
  } else {
    throw "INVALID TYPE IN generate_deserialize_container '" + ttype->get_name() + "' for prefix '" + prefix + "'";
  }

  // For loop iterates over elements
  string i = tmp("_i");
  out <<
    indent() << "for " << i << ":= 0; " << i << " < " << size << "; " << i << "++ {" << endl;
  indent_up();

  if (ttype->is_map()) {
    generate_deserialize_map_element(out, (t_map*)ttype, declare, prefix);
  } else if (ttype->is_set()) {
    generate_deserialize_set_element(out, (t_set*)ttype, declare, prefix);
  } else if (ttype->is_list()) {
    generate_deserialize_list_element(out, (t_list*)ttype, declare, prefix);
  }

  indent_down();
  out <<
    indent() << "}" << endl;
  // Read container end
  if (ttype->is_map()) {
    out <<
      indent() << err << " = iprot.ReadMapEnd();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" 
                        << -1
                        << ", \"" << escape_string(((t_map*)ttype)->get_cpp_name())
                        << "\", " << "\"map\", " << err << "); }" << endl;
  } else if (ttype->is_set()) {
    out <<
      indent() << err << " = iprot.ReadSetEnd();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" 
                        << -1
                        << ", \"" << escape_string(((t_set*)ttype)->get_cpp_name())
                        << "\", " << "\"set\", " << err << "); }" << endl;
  } else if (ttype->is_list()) {
    out <<
      indent() << err << " = iprot.ReadListEnd();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionReadField(" 
                        << -1
                        << ", \"" << escape_string(((t_list*)ttype)->get_cpp_name())
                        << "\", " << "\"list\"," << err << "); }" << endl;
  }
}


/**
 * Generates code to deserialize a map
 */
void t_go_generator::generate_deserialize_map_element(ofstream &out,
                                                       t_map* tmap,
                                                       bool   declare,
                                                       string prefix,
                                                       string err) {
  string key = tmp("_key");
  string val = tmp("_val");
  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);

  generate_deserialize_field(out, &fkey, true);
  generate_deserialize_field(out, &fval, true);

  indent(out) <<
    prefix << ".Set(" << key << ", " << val << ")" << endl;
}

/**
 * Write a set element
 */
void t_go_generator::generate_deserialize_set_element(ofstream &out,
                                                       t_set* tset,
                                                       bool   declare,
                                                       string prefix,
                                                       string err) {
  string elem = tmp("_elem");
  t_field felem(tset->get_elem_type(), elem);

  generate_deserialize_field(out, &felem, true, "", err);

  indent(out) <<
    prefix << ".Add(" << elem << ")" << endl;
}

/**
 * Write a list element
 */
void t_go_generator::generate_deserialize_list_element(ofstream &out,
                                                        t_list* tlist,
                                                        bool   declare,
                                                        string prefix,
                                                        string err) {
  string elem = tmp("_elem");
  t_field felem(tlist->get_elem_type(), elem);

  generate_deserialize_field(out, &felem, true, "", err);

  indent(out) <<
    prefix << ".Push(" << elem << ")" << endl;
}


/**
 * Serializes a field of any type.
 *
 * @param tfield The field to serialize
 * @param prefix Name to prepend to field name
 */
void t_go_generator::generate_serialize_field(ofstream &out,
                                               t_field* tfield,
                                               string prefix,
                                               string err) {
  t_type* type = get_true_type(tfield->get_type());
  string name(prefix + publicize(tfield->get_name()));

  // Do nothing for void types
  if (type->is_void()) {
    throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + name;
  }

  if (type->is_struct() || type->is_xception()) {
    generate_serialize_struct(out,
                              (t_struct*)type,
                              name,
                              err);
  } else if (type->is_container()) {
    generate_serialize_container(out,
                                 type,
                                 name,
                                 err);
  } else if (type->is_base_type() || type->is_enum()) {

    indent(out) <<
      err << " = oprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw
          "compiler error: cannot serialize void field in a struct: " + name;
        break;
      case t_base_type::TYPE_STRING:
        out << "WriteString(" << name << ")";
        break;
      case t_base_type::TYPE_BOOL:
        out << "WriteBool(bool(" << name << "))";
        break;
      case t_base_type::TYPE_BYTE:
        out << "WriteByte(byte(" << name << "))";
        break;
      case t_base_type::TYPE_I16:
        out << "WriteI16(int16(" << name << "))";
        break;
      case t_base_type::TYPE_I32:
        out << "WriteI32(int32(" << name << "))";
        break;
      case t_base_type::TYPE_I64:
        out << "WriteI64(int64(" << name << "))";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "WriteDouble(float64(" << name << "))";
        break;
      default:
        throw "compiler error: no PHP name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "WriteI32(int32(" << name << "))";
    }
    string structName = (prefix.size()) ? prefix + "ThriftName()" : "\"\"";
    out << ";" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << tfield->get_key()
                        << ", \"" << escape_string(tfield->get_name())
                        << "\", " << structName << ", " << err << "); }\n";
  } else {
    throw "INVALID TYPE IN generate_serialize_field '" + type->get_name() + "' for field '" + prefix + publicize(tfield->get_name()) + "'";
  }
}

/**
 * Serializes all the members of a struct.
 *
 * @param tstruct The struct to serialize
 * @param prefix  String prefix to attach to all fields
 */
void t_go_generator::generate_serialize_struct(ofstream &out,
                                               t_struct* tstruct,
                                               string prefix,
                                               string err) {
  out <<
    indent() << err << " = " << prefix << ".Write(oprot);" << endl <<
    indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteStruct(" 
                      << "\"" << escape_string(tstruct->get_name()) << "\", " << err << "); }\n";
}

void t_go_generator::generate_serialize_container(ofstream &out,
                                                  t_type* ttype,
                                                  string prefix,
                                                  string err) {
  if (ttype->is_map()) {
    out <<
      indent() << err << " = oprot.WriteMapBegin(" <<
                         type_to_enum(((t_map*)ttype)->get_key_type()) << ", " <<
                         type_to_enum(((t_map*)ttype)->get_val_type()) << ", " <<
                         prefix << ".Len())" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"map\", " << err << "); }\n";
  } else if (ttype->is_set()) {
    out <<
      indent() << err << " = oprot.WriteSetBegin(" <<
                         type_to_enum(((t_set*)ttype)->get_elem_type()) << ", " <<
                         prefix << ".Len())" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"set\", " << err << "); }\n";
  } else if (ttype->is_list()) {
    out <<
      indent() << err << " = oprot.WriteListBegin(" <<
                         type_to_enum(((t_list*)ttype)->get_elem_type()) << ", " <<
                         prefix << ".Len())" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"list\", " << err << "); }\n";
  } else {
    throw "INVALID TYPE IN generate_serialize_container '" + ttype->get_name() + "' for prefix '" + prefix + "'";
  }

  if (ttype->is_map()) {
    string miter = tmp("Miter");
    string kiter = tmp("Kiter");
    string viter = tmp("Viter");
    t_map* tmap = (t_map*)ttype;
    out <<
      indent() << "for " << miter << " := range " << prefix << ".Iter() {" << endl <<
      indent() << "  " << kiter << ", " << viter << " := " << miter << ".Key().(" << type_to_go_type(tmap->get_key_type()) << "), " << miter << ".Value().(" << type_to_go_type(tmap->get_val_type()) << ")" << endl;
    indent_up();
    generate_serialize_map_element(out, tmap, kiter, viter);
    indent_down();
    indent(out) << "}" << endl;
  } else if (ttype->is_set()) {
    t_set* tset = (t_set*)ttype;
    string iter = tmp("Iter");
    string iter2 = tmp("Iter");
    out <<
      indent() << "for " << iter << " := range " << prefix << ".Iter() {" << endl <<
      indent() << "  " << iter2 << " := " << iter << ".(" << type_to_go_type(tset->get_elem_type()) << ")" << endl;
    indent_up();
    generate_serialize_set_element(out, tset, iter2);
    indent_down();
    indent(out) << "}" << endl;
  } else if (ttype->is_list()) {
    t_list* tlist = (t_list*)ttype;
    string iter = tmp("Iter");
    string iter2 = tmp("Iter");
    out <<
      indent() << "for " << iter << " := range " << prefix << ".Iter() {" << endl <<
      indent() << "  " << iter2 << " := " << iter << ".(" << type_to_go_type(tlist->get_elem_type()) << ")" << endl;
    indent_up();
    generate_serialize_list_element(out, tlist, iter2);
    indent_down();
    indent(out) << "}" << endl;
  }

  if (ttype->is_map()) {
    out <<
      indent() << err << " = oprot.WriteMapEnd();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"map\", " << err << "); }\n";
  } else if (ttype->is_set()) {
    out <<
      indent() << err << " = oprot.WriteSetEnd();" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"set\", " << err << "); }\n";
  } else if (ttype->is_list()) {
    out <<
      indent() << err << " = oprot.WriteListEnd()" << endl <<
      indent() << "if " << err << " != nil { return protocol.NewTProtocolExceptionWriteField(" 
                        << -1
                        << ", \"" << escape_string(ttype->get_name())
                        << "\", " << "\"list\", " << err << "); }\n";
  }
}

/**
 * Serializes the members of a map.
 *
 */
void t_go_generator::generate_serialize_map_element(ofstream &out,
                                                     t_map* tmap,
                                                     string kiter,
                                                     string viter,
                                                     string err) {
  t_field kfield(tmap->get_key_type(), kiter);
  generate_serialize_field(out, &kfield, "", err);

  t_field vfield(tmap->get_val_type(), viter);
  generate_serialize_field(out, &vfield, "", err);
}

/**
 * Serializes the members of a set.
 */
void t_go_generator::generate_serialize_set_element(ofstream &out,
                                                     t_set* tset,
                                                     string iter,
                                                     string err) {
  t_field efield(tset->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "", err);
}

/**
 * Serializes the members of a list.
 */
void t_go_generator::generate_serialize_list_element(ofstream &out,
                                                      t_list* tlist,
                                                      string iter,
                                                      string err) {
  t_field efield(tlist->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "", err);
}

/**
 * Generates the docstring for a given struct.
 */
void t_go_generator::generate_go_docstring(ofstream& out,
                                               t_struct* tstruct) {
  generate_go_docstring(out, tstruct, tstruct, "Attributes");
}

/**
 * Generates the docstring for a given function.
 */
void t_go_generator::generate_go_docstring(ofstream& out,
                                               t_function* tfunction) {
  generate_go_docstring(out, tfunction, tfunction->get_arglist(), "Parameters");
}

/**
 * Generates the docstring for a struct or function.
 */
void t_go_generator::generate_go_docstring(ofstream& out,
                                               t_doc*    tdoc,
                                               t_struct* tstruct,
                                               const char* subheader) {
  bool has_doc = false;
  stringstream ss;
  if (tdoc->has_doc()) {
    has_doc = true;
    ss << tdoc->get_doc();
  }

  const vector<t_field*>& fields = tstruct->get_members();
  if (fields.size() > 0) {
    if (has_doc) {
      ss << endl;
    }
    has_doc = true;
    ss << subheader << ":\n";
    vector<t_field*>::const_iterator p_iter;
    for (p_iter = fields.begin(); p_iter != fields.end(); ++p_iter) {
      t_field* p = *p_iter;
      ss << " - " << publicize(p->get_name());
      if (p->has_doc()) {
        ss << ": " << p->get_doc();
      } else {
        ss << endl;
      }
    }
  }

  if (has_doc) {
    generate_docstring_comment(out,
      "/**\n",
      " * ", ss.str(),
      " */\n");
  }
}

/**
 * Generates the docstring for a generic object.
 */
void t_go_generator::generate_go_docstring(ofstream& out,
                                               t_doc* tdoc) {
  if (tdoc->has_doc()) {
    generate_docstring_comment(out,
      "/**\n",
      " *", tdoc->get_doc(),
      " */\n");
  }
}

/**
 * Declares an argument, which may include initialization as necessary.
 *
 * @param tfield The field
 */
string t_go_generator::declare_argument(t_field* tfield) {
  std::ostringstream result;
  result << publicize(tfield->get_name()) << "=";
  if (tfield->get_value() != NULL) {
    result << "thrift_spec[" <<
      tfield->get_key() << "][4]";
  } else {
    result << "nil";
  }
  return result.str();
}

/**
 * Renders a field default value, returns nil otherwise.
 *
 * @param tfield The field
 */
string t_go_generator::render_field_default_value(t_field* tfield) {
  t_type* type = get_true_type(tfield->get_type());
  if (tfield->get_value() != NULL) {
    return render_const_value(type, tfield->get_value());
  } else {
    return "nil";
  }
}

/**
 * Renders a function signature of the form 'type name(args)'
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_go_generator::function_signature(t_function* tfunction,
                                          string prefix) {
  // TODO(mcslee): Nitpicky, no ',' if argument_list is empty
  return
    publicize(prefix + tfunction->get_name()) + 
      "(" + argument_list(tfunction->get_arglist()) + ")";
}

/**
 * Renders an interface function signature of the form 'type name(args)'
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_go_generator::function_signature_if(t_function* tfunction,
                                             string prefix,
                                             bool addOsError) {
  // TODO(mcslee): Nitpicky, no ',' if argument_list is empty
  string signature = publicize(prefix + tfunction->get_name()) + "(";
  signature += argument_list(tfunction->get_arglist()) + ") (";
  t_type* ret = tfunction->get_returntype();
  //t_struct* exceptions = tfunction->get_xceptions();
  //string errs = argument_list(exceptions);
  string retval(tmp("retval"));
  if(!ret->is_void()) {
    signature += retval + " " + type_to_go_type(ret);
    if(addOsError) {// || errs.size()==0) {
      signature += ", ";
    }
  }
  /*
  if(errs.size()>0) {
    signature += errs;
    if(addOsError)
      signature += ", ";
  }
  */
  if(addOsError) {
    signature += "err os.Error";
  }
  signature += ")";
  return signature;
}


/**
 * Renders a field list
 */
string t_go_generator::argument_list(t_struct* tstruct) {
  string result = "";
  
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      result += ", ";
    }
    result += (*f_iter)->get_name() + " " + type_to_go_type((*f_iter)->get_type());
  }
  return result;
}

string t_go_generator::type_name(t_type* ttype) {
  t_program* program = ttype->get_program();
  if (ttype->is_service()) {
    return get_real_go_module(program) + "." + ttype->get_name();
  }
  if (program != NULL && program != program_) {
    return get_real_go_module(program) + ".ttypes." + ttype->get_name();
  }
  return ttype->get_name();
}

/**
 * Converts the parse type to a go tyoe
 */
string t_go_generator::type_to_enum(t_type* type) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";
    case t_base_type::TYPE_STRING:
      return "protocol.STRING";
    case t_base_type::TYPE_BOOL:
      return "protocol.BOOL";
    case t_base_type::TYPE_BYTE:
      return "protocol.BYTE";
    case t_base_type::TYPE_I16:
      return "protocol.I16";
    case t_base_type::TYPE_I32:
      return "protocol.I32";
    case t_base_type::TYPE_I64:
      return "protocol.I64";
    case t_base_type::TYPE_DOUBLE:
      return "protocol.DOUBLE";
    }
  } else if (type->is_enum()) {
    return "protocol.I32";
  } else if (type->is_struct() || type->is_xception()) {
    return "protocol.STRUCT";
  } else if (type->is_map()) {
    return "protocol.MAP";
  } else if (type->is_set()) {
    return "protocol.SET";
  } else if (type->is_list()) {
    return "protocol.LIST";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

/**
 * Converts the parse type to a go tyoe
 */
string t_go_generator::type_to_go_type(t_type* type) {
  //type = get_true_type(type);
  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "";
    case t_base_type::TYPE_STRING:
      return "string";
    case t_base_type::TYPE_BOOL:
      return "bool";
    case t_base_type::TYPE_BYTE:
      return "byte";
    case t_base_type::TYPE_I16:
      return "int16";
    case t_base_type::TYPE_I32:
      return "int32";
    case t_base_type::TYPE_I64:
      return "int64";
    case t_base_type::TYPE_DOUBLE:
      return "float64";
    }
  } else if (type->is_enum()) {
    return publicize(type->get_name());
  } else if (type->is_struct() || type->is_xception()) {
    return string("*") + publicize(type->get_name());
  } else if (type->is_map()) {
    return "protocol.TMap";
    //t_map* t = (t_map*)type;
    //string keyType = type_to_go_type(t->get_key_type());
    //string valueType = type_to_go_type(t->get_val_type());
    //return string("map[") + keyType + "]" + valueType;
  } else if (type->is_set()) {
    return "protocol.TSet";
    //t_set* t = (t_set*)type;
    //string elemType = type_to_go_type(t->get_elem_type());
    //return string("[]") + elemType;
  } else if (type->is_list()) {
    return "protocol.TList";
    //t_list* t = (t_list*)type;
    //string elemType = type_to_go_type(t->get_elem_type());
    //return string("[]") + elemType;
  } else if (type->is_typedef()) {
    return ((t_typedef*)type)->get_symbolic();
  }

  throw "INVALID TYPE IN type_to_go_type: " + type->get_name();
}


/**
 * Converts the parse type to a go tyoe
 */
bool t_go_generator::can_be_nil(t_type* type) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "Invalid Type for can_be_nil";
    case t_base_type::TYPE_STRING:
    case t_base_type::TYPE_BOOL:
    case t_base_type::TYPE_BYTE:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
    case t_base_type::TYPE_DOUBLE:
      return false;
    }
  } else if (type->is_enum()) {
    return false;
  } else if (type->is_struct() || type->is_xception()) {
    return true;
  } else if (type->is_map()) {
    return true;
  } else if (type->is_set()) {
    return true;
  } else if (type->is_list()) {
    return true;
  }

  throw "INVALID TYPE IN can_be_nil: " + type->get_name();
}



/** See the comment inside generate_go_struct_definition for what this is. */
string t_go_generator::type_to_spec_args(t_type* ttype) {
  while (ttype->is_typedef()) {
    ttype = ((t_typedef*)ttype)->get_type();
  }

  if (ttype->is_base_type() || ttype->is_enum()) {
    return "nil";
  } else if (ttype->is_struct() || ttype->is_xception()) {
    return "(" + type_name(ttype) + ", " + type_name(ttype) + ".thrift_spec)";
  } else if (ttype->is_map()) {
    return "(" +
      type_to_enum(((t_map*)ttype)->get_key_type()) + "," +
      type_to_spec_args(((t_map*)ttype)->get_key_type()) + "," +
      type_to_enum(((t_map*)ttype)->get_val_type()) + "," +
      type_to_spec_args(((t_map*)ttype)->get_val_type()) +
      ")";

  } else if (ttype->is_set()) {
    return "(" +
      type_to_enum(((t_set*)ttype)->get_elem_type()) + "," +
      type_to_spec_args(((t_set*)ttype)->get_elem_type()) +
      ")";

  } else if (ttype->is_list()) {
    return "(" +
      type_to_enum(((t_list*)ttype)->get_elem_type()) + "," +
      type_to_spec_args(((t_list*)ttype)->get_elem_type()) +
      ")";
  }

  throw "INVALID TYPE IN type_to_spec_args: " + ttype->get_name();
}


THRIFT_REGISTER_GENERATOR(go, "Go",
""
);