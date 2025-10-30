#pragma once
#include <covscript/context/context.hpp>
#include <covscript/compiler/compiler.hpp>

namespace cs
{
	class op_parser_t
	{
		map_t<std::string, callable> literals;
		memory_manager &storage;

	   public:
		explicit op_parser_t(const context_t &cxt) : storage(cxt->memory) {}

		void add_string_literal(const std::string &literal, const callable &func)
		{
			if (literals.count(literal) > 0)
				throw runtime_error("Duplicated String Literal.");
			else
				literals.emplace(literal, func);
		}

		var get_string_literal(const std::string &data, const std::string &literal)
		{
			if (literals.count(literal) > 0)
			{
				fwd_array arg{data};
				return literals.at(literal).call(arg);
			}
			else
				throw runtime_error("Undefined String Literal.");
		}

		var parse_add(const var &, const var &);

		var parse_addasi(var, const var &);

		var parse_sub(const var &, const var &);

		var parse_subasi(var, const var &);

		var parse_minus(const var &);

		var parse_mul(const var &, const var &);

		var parse_mulasi(var, const var &);

		var parse_escape(const var &);

		var parse_div(const var &, const var &);

		var parse_divasi(var, const var &);

		var parse_mod(const var &, const var &);

		var parse_modasi(var, const var &);

		var parse_pow(const var &, const var &);

		var parse_powasi(var, const var &);

		var &parse_dot_lhs(const var &, token_base *);

		var parse_dot(const var &, token_base *);

		var parse_arrow(const var &, token_base *);

		var parse_typeid(const var &);

		var parse_new(const var &);

		var parse_gcnew(const var &);

		var parse_und(const var &, const var &);

		var parse_abo(const var &, const var &);

		var parse_ueq(const var &, const var &);

		var parse_aeq(const var &, const var &);

		var parse_asi(var, const var &);

		var parse_lnkasi(var &, const var &);

		var parse_lnkasi(binary_tree<token_base *>::iterator, const var &);

		var parse_bind(token_base *, const var &);

		var parse_choice(const var &, const binary_tree<token_base *>::iterator &);

		var parse_pair(const var &, const var &);

		var parse_equ(const var &, const var &);

		var parse_neq(const var &, const var &);

		var parse_and(const binary_tree<token_base *>::iterator &, const binary_tree<token_base *>::iterator &);

		var parse_or(const binary_tree<token_base *>::iterator &, const binary_tree<token_base *>::iterator &);

		var parse_not(const var &);

		var parse_inc(const var &, const var &);

		var parse_dec(const var &, const var &);

		var parse_addr(const var &);

		var parse_fcall(const var &, token_base *);

		var &parse_access_lhs(const var &, const var &);

		var parse_access(const var &, const var &);

		var parse_expr(const binary_tree<token_base *>::iterator &, bool = false);
	};
} // namespace cs