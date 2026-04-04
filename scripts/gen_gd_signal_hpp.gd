# GDSignal (TypedSignal) Header Generator
#
# This script generates a C++ TypedSignal definition for every class and every standard signal
# registered in Godot's ClassDB.
#
# Purpose:
# - Automatically generate TypedSignal wrappers for all built-in Godot signals.
# - Makes working with Godot signals in C++ (GDExtension) easier and safer.
# - Does not include dynamically added user signals; only the standard signals provided by the
#   engine.
#
# Notes:
# - It is recommended to regenerate the file whenever the editor version changes.
#   This ensures that the signals are consistent with the Godot engine version.
# - Generate the file using the same editor that you use to export your app and GDExtensions with
#   godot-cpp.
#
# Usage:
# 1. Create an empty Godot project.
# 2. Add a main scene with a root node of type 'Node'.
# 3. Attach this script to the root node.
# 4. Press Play.
# 5. The absolute path of the generated header file will be printed to the console.
# 6. Optionally, you can customize the output path using the OUTPUT_FILE_PATH constant.
#
# Output:
# - A C++ header file containing TypedSignal definitions for all class signals.
# - Can be included directly in your godot-cpp project.
#
# Note: The generated header depends on "godot_cpp_util/typed_signal.hpp" and must be included in
# your C++ project before using the TypedSignal definitions.



extends Node



const OUTPUT_FILE_PATH: String = "res://gen/include/godot_cpp_util/gd_signal.hpp"



func _ready() -> void:
	var class_data := collect_class_data()
	write_gd_signal_header(class_data, OUTPUT_FILE_PATH)



class ArgData:
	var type: int = -1
	var name: String = "<no name>"
	var object_class_name: String = ""



class SignalData:
	var name: String = "<no name>"
	var args: Array[ArgData] = []



class ClassData:
	var name: String = "<no name>"
	var signals: Array[SignalData] = []



class GodotCppTypeData:
	var header: String = "<no header>"
	var name: String = "<no name>"
	var namespace_: String = "<no namespace>"
	var needs_forward_class_declaration: bool = false
	
	
	
	func _init(variant_type: Variant.Type):
		match variant_type:
			Variant.Type.TYPE_NIL:
				name = "Variant"
				namespace_ = "godot::"
				header = ""
			
			Variant.Type.TYPE_BOOL:
				name = "bool"
				namespace_ = ""
				header = ""
			
			Variant.Type.TYPE_INT:
				name = "int64_t"
				namespace_ = ""
				header = "<cstdint>"
			
			Variant.Type.TYPE_FLOAT:
				name = "real_t"
				namespace_ = "godot::"
				header = "<godot_cpp/core/math_defs.hpp>"
			
			_:
				if variant_type == TYPE_MAX:
					name = "Variant"
				else:
					name = type_string(variant_type)
					if name.is_empty():
						name = "Variant"
				
				namespace_ = "godot::"
				header = ""
				needs_forward_class_declaration = true



func collect_class_data() -> Array[ClassData]:
	var class_data_array: Array[ClassData] = []
	var classes := ClassDB.get_class_list()
	for class_name_ in classes:
		var signals := ClassDB.class_get_signal_list(class_name_)
		if signals.is_empty():
			continue
		
		var class_data := ClassData.new()
		class_data.name = class_name_
		
		for sig in signals:
			var signal_data := SignalData.new()
			signal_data.name = sig.name
			
			for arg in sig.args:
				var arg_data := ArgData.new()
				arg_data.type = arg.type
				arg_data.name = arg.name
				if arg.type == Variant.Type.TYPE_OBJECT:
					if arg.class_name.is_empty():
						arg_data.object_class_name = "Object"
					else:
						arg_data.object_class_name = arg.class_name
				
				signal_data.args.push_back(arg_data)
			
			class_data.signals.push_back(signal_data)
		
		class_data_array.push_back(class_data)
	
	return class_data_array



func write_gd_signal_header(class_data_array: Array[ClassData], output_path: String) -> void:
	# Create parent folder.
	var parent_dir_path := output_path.get_base_dir()
	var parent_dir := DirAccess.open(output_path.get_base_dir())
	if not parent_dir:
		var err := DirAccess.make_dir_recursive_absolute(parent_dir_path)
		if err != Error.OK:
			push_error(
				"Cannot create directories for path (Error: %d): %s" % [err, parent_dir_path]
			)
			return
	
	# Now open the file.
	var file := FileAccess.open(output_path, FileAccess.WRITE)
	if not file:
		push_error("Cannot open file: %s" % output_path)
		return
	
	# Collect includes and forward class declarations.
	var includes: Array[String] = []
	var forward_class_declarations: Array[String] = []
	for class_data in class_data_array:
		for sig in class_data.signals:
			for arg in sig.args:
				var cpp_data := GodotCppTypeData.new(arg.type)
				
				var header := cpp_data.header
				if not header.is_empty() and not includes.has(header):
					includes.push_back(header)
				
				var cpp_type := cpp_data.name
				if arg.type == Variant.Type.TYPE_OBJECT:
					cpp_type = arg.object_class_name
				
				if (
					cpp_data.needs_forward_class_declaration
					and not cpp_type.is_empty()
					and not forward_class_declarations.has(cpp_type)
				):
					forward_class_declarations.push_back(cpp_type)
	
	# Write file.
	file.store_line("// Generated by Godot Editor: '%s'" % Engine.get_version_info().string)
	file.store_line("")
	file.store_line("")
	file.store_line("")
	
	file.store_line("#pragma once")
	file.store_line("")
	file.store_line("")
	file.store_line("")
	file.store_line('#include "godot_cpp_util/typed_signal.hpp"')
	file.store_line("")
	for i in includes:
		file.store_line('#include %s' % i)
	file.store_line("")
	file.store_line("")
	file.store_line("")
	if not forward_class_declarations.is_empty():
		file.store_line("namespace godot {")
		file.store_line("")
		for d in forward_class_declarations:
			file.store_line('class %s;' % d)
		file.store_line("")
		file.store_line("} // namespace godot")
		file.store_line("")
		file.store_line("")
		file.store_line("")
	file.store_line("namespace godot::GDSignal {")
	file.store_line("")
	
	var is_first_entry: bool = true
	for class_data in class_data_array:
		if not is_first_entry:
			file.store_line("")
			file.store_line("")
		
		file.store_line("struct %s {" % class_data.name)
		
		for sig in class_data.signals:
			if sig.args.is_empty():
				file.store_line(
					'    static constexpr TypedSignal<"%s"> %s{};' % [sig.name, sig.name]
				)
			else:
				var arg_list := []
				for arg in sig.args:
					var cpp_type_data := GodotCppTypeData.new(arg.type)
					var cpp_type := cpp_type_data.name
					if arg.type == Variant.Type.TYPE_OBJECT:
						cpp_type = "%s*" % arg.object_class_name
						
					if not cpp_type_data.namespace_.is_empty():
						cpp_type = cpp_type_data.namespace_ + cpp_type
					
					arg_list.append("%s /* %s */" % [cpp_type, arg.name])
				
				var args_str := ", ".join(arg_list)
				file.store_line(
					'    static constexpr TypedSignal<"%s", %s> %s{};'
						% [sig.name, args_str, sig.name]
				)
		
		file.store_line("};")
		file.store_line("")
		
		is_first_entry = false
	
	file.store_line("} // namespace godot::GDSignal")
	file.close()
	
	var abs_path := ProjectSettings.globalize_path(output_path)
	print("Header written to: '%s'" % abs_path)
