/**
 The One Programming Language

 File: parser/parser.c
  _        _
 / \ |\ | |_    Max Base
 \_/ | \| |_    Copyright 2021; One Language Contributors

 **/

#include "parser.h"

// Global variable(s)
Parser parser;

/*
 * @function: parser_init
 * @description: Set default value for the global `parser` variable
 * @arguments: char* filepath, char* input, Token** tokens
 * @return: void; nothing
 */
void parser_init(char* filepath, char* input, Token** tokens)
{
	debug_parser("parser_init");

	parser.path = filepath;
	parser.data = input;
	parser.tokens = tokens;
}

/*
 * @function: parser_scan_import
 * @description: scan import statement
 * @arguments: nothing
 * @return: AstImportDeclaration
 */
AstImportDeclaration* parser_scan_import()
{
	debug_parser("parser_scan_import");

	// IMPORT <skip> <names> (| { <symbols> } )
	AstImportDeclaration* ast = malloc(sizeof(AstImportDeclaration));

	ast->names = malloc(sizeof(AstImportNameArray));
	array_init(ast->names);

	ast->symbols = malloc(sizeof(AstImportSymbolArray));
	array_init(ast->symbols);

	parser_token_expect(TOKEN_IMPORT);

	parser_token_skip();

	Token* value = parser_token_get();
	debug_parser("parser_scan_import: current token is %s", token_name(value->type));
	parser_token_expect(TOKEN_VALUE_IDENTIFIER);
	info_parser("parser_scan_import: %s", value->value);

	parser_token_skip();

	// AS <skip> IDENTIFIER
	if (parser_token_has(TOKEN_AS))
	{
		parser_token_skip();

		Token* alias = parser_token_get();
		debug_parser("parser_scan_import: alias current token is %s", token_name(alias->type));

		parser_token_expect(TOKEN_VALUE_IDENTIFIER);
		ast->alias = strdup(alias->value);
		free(alias);
		info_parser("parser_scan_import: set alias as %s", ast->alias);
	}

	parser_token_skip();

	// { <skip> [[<names> <skip> AS <skip> IDENTIFIER ]]  }
	if (parser_token_has(TOKEN_OPERATOR_BRACKET_CURLY_LEFT))
	{
		parser_token_skip();

		AstImportSymbol* symbol = malloc(sizeof(AstImportSymbol));

		Token* symbol_name = parser_token_get();
		parser_token_expect(TOKEN_VALUE_IDENTIFIER);

		symbol->names = malloc(sizeof(StringArray));

		array_init(symbol->names);
		array_push(symbol->names, strdup(symbol_name->value));
		free(symbol_name);

		parser_token_skip();

		if (parser_token_has(TOKEN_AS))
		{
			symbol->has_alias = true;
			parser_token_skip();
			Token* symbol_alias = parser_token_get();
			parser_token_expect(TOKEN_VALUE_IDENTIFIER);
			symbol->alias = strdup(symbol_alias->value);
			free(symbol_alias);
		}
		else
		{
			symbol->has_alias = false;
		}

		parser_token_expect(TOKEN_OPERATOR_BRACKET_CURLY_RIGHT);
	}

	return ast;
}

/*
 * @function: parser_scan_fn
 * @description: scan fn statement
 * @arguments: nothing
 * @return: AstFunctionDeclaration
 */
AstFunctionDeclaration* parser_scan_fn()
{
	debug_parser("parser_scan_fn");

	// FN <skip> IDENTIFIER
	AstFunctionDeclaration* ast = malloc(sizeof(AstFunctionDeclaration));

	parser_token_expect(TOKEN_FN);

	parser_token_skip();

	Token* name = parser_token_get();
	parser_token_expect(TOKEN_VALUE_IDENTIFIER);
	ast->name = strdup(name->value);
	free(name);
	info_parser("parser_scan_fn: name is %s", ast->name);

	exit(0); // TODO

	return ast;
}

/*
 * @function: parser_scan_package
 * @description: scan package statement
 * @arguments: nothing
 * @return: AstPackage
 */
AstPackage* parser_scan_package()
{
	debug_parser("parser_scan_package");

	// PACKAGE <skip> <name>
	AstPackage* ast = malloc(sizeof(AstPackage));

	parser_token_expect(TOKEN_PACKAGE);

	parser_token_skip();

	Token* value = parser_token_get();
	debug_parser("parser_scan_package: current token is %s", token_name(parser_token_get_type()));
	parser_token_expect(TOKEN_VALUE_IDENTIFIER);
	info_parser("parser_scan_package: %s", value->value);

	ast->name = strdup(value->value);

	free(value);
	// printf("==>%s\n", ast->name);

	return ast;
}

/*
 * @function: parser_scan
 * @description: Start to check tokens. it's entry point of parser stage.
 * @arguments: nothing
 * @return: Always a pointer of AST (AstFile) struct
 */
AstFile* parser_scan()
{
	debug_parser("parser_scan");

	AstFile* ast = malloc(sizeof(AstFile));

	parser_token_skip();

	if (parser_token_get_type() == TOKEN_PACKAGE)
	{
		ast->module = parser_scan_package();
	}

	TokenType type = parser_token_get_type();

	while (type != TOKEN_EOF && type != TOKEN_ERROR)
	{
		if (parser_token_skip())
		{
			type = parser_token_get_type();
		}

		if (type == TOKEN_IMPORT)
		{
			parser_scan_import();
		}
		else if (type == TOKEN_FN)
		{
			parser_scan_fn();
		}
		else
		{
			error_parser("%s is unknown statement in the file!", token_name(type));
		}
		type = parser_token_get_type();
	}

	return ast;
}

/*
 * @function: parser_trace
 * @description: Log and trace items of tokens
 * @arguments: FILE* file_out, char* data, Token** tokens, AstFile* ast
 * @return: nothing, void
 */
void parser_trace(FILE* file_out, char* data, Token** tokens, AstFile* ast)
{
	debug_parser("parser_trace");
}

/*
 * @function: parser_free
 * @description: Free allocated memory for the parser stage! TODO
 * @arguments: nothing
 * @return: nothing, void
 */
void parser_free()
{
	debug_parser("parser_free");
}
