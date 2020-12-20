#include "dotenv.h"

#include "environ.h"
#include "Parser.h"

#include <fstream>
#include <utility>


using namespace std;
using namespace dotenv;


dotenvFacade& dotenvFacade::load_dotenv(const string& dotenv_path, const bool overwrite, const bool interpolate)
{
    ifstream env_file;
    env_file.open(dotenv_path);

    if (env_file.good())
    {
        Parser parser;
        parser.parse(env_file, overwrite, interpolate);
        env_file.close();
    }

    return *this;
}


dotenvFacade::value_type dotenvFacade::operator[](const key_type& k) const
{
    return getenv(k).second;
}


dotenvFacade& dotenvFacade::instance()
{
    return _instance;
}


const string dotenvFacade::env_filename = ".env";
dotenvFacade dotenvFacade::_instance;

dotenvFacade& dotenv::env = dotenvFacade::instance();
