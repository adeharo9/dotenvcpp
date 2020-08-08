#include "ReferencesListener.h"

#include "environ.h"

#include <utility>


using namespace dotenv;
using namespace std;


ReferencesListener::ReferencesListener(const string& key, ReferencesTable& references_table, SymbolsTable& symbols_table):
    key(key),
    references_table(references_table),
    symbols_table(symbols_table)
{

}


void ReferencesListener::exitVariable(LineParser::VariableContext* ctx)
{
    size_t pos = ctx->getStart()->getCharPositionInLine();
    string var_name;

    // Get variable name and positional info
    if (ctx->BOUNDED_VARIABLE() != nullptr)
    {
        var_name += ctx->BOUNDED_VARIABLE()->getText();
        var_name = var_name.substr(2, var_name.size() - 3);
    }
    else if (ctx->UNBOUNDED_VARIABLE() != nullptr)
    {
        var_name += ctx->UNBOUNDED_VARIABLE()->getText();
        var_name = var_name.substr(1, var_name.size() - 1);
    }

    // If the symbol does not exist on the table, it is not local (i.e. it is
    // defined on the outer environment)
    // Retrieve it from the environment and register it in the symbols table
    // If it does not exist, add it as not complete in the symbols table
    if (symbols_table.find(var_name) == symbols_table.end())
    {
        pair<bool, string> result = getenv(var_name);
        SymbolRecord var(result.first, result.first, 0, false);

        // Only add value if it had one
        if (result.first)
        {
            var.set_value(result.second);
        }
        symbols_table.emplace(var_name, var);
    }

    // Add one dependency
    SymbolRecord& symbol_record = symbols_table.at(key);
    symbol_record.dependency_add_one();

    ReferenceRecord reference_record(symbol_record.line(), symbol_record.offset() + pos);
    references_table.emplace(var_name, reference_record);
}
