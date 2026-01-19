#include "../include/argparse.hpp"
#include "../include/common.hpp"
#include "../include/crypto.hpp"
#include "../include/userActions.hpp"

int main ( int argc, char **argv )
{
  argparse::ArgumentParser program ( "passwordKeeper" );

  program.add_argument ( "--user" ).required ();

  program.add_argument ( "--userPassword" ).required ();

  program.add_argument ( "--login" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--register" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--changePassword" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--newPassword" ).default_value ( std::string{ "" } );

  program.add_argument ( "--addCreds" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--deleteCreds" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--viewCreds" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--changeCreds" ).default_value ( false ).implicit_value ( true );

  program.add_argument ( "--siteName" ).default_value ( std::string{ "" } );

  program.add_argument ( "--sitePassword" ).default_value ( std::string{ "" } );

  program.add_argument ( "--secretKey" ).default_value ( std::string{ "" } );

  program.add_argument ( "--changeSiteCreds" ).default_value ( false ).implicit_value ( true );

  try
  {
    program.parse_args ( argc, argv );
  }
  catch ( const std::exception &err )
  {
    std::cerr << err.what () << std::endl;
    return 1;
  }

  std::string user{ program.get<std::string> ( "--user" ) };
  std::string userPassword{ program.get<std::string> ( "--userPassword" ) };
  std::string newPassword{ program.get<std::string> ( "--newPassword" ) };
  std::string siteName{ program.get<std::string> ( "--siteName" ) };
  std::string sitePassword{ program.get<std::string> ( "--sitePassword" ) };
  std::string secretKey{ program.get<std::string> ( "--secretKey" ) };

  if ( program.get<bool> ( "--register" ) )
  {
    createUser ( user, userPassword );
  }

  if ( program.get<bool> ( "--login" ) )
  {
    if ( authenticateUser ( user, userPassword ) )
    {
      std::cerr << "Successfully authenticated as: " << user << '\n';

      if ( program.get<bool> ( "--changePassword" ) )
      {
        if ( newPassword.empty () )
        {
          std::cerr << "--changePassword requires --newPassword" << std::endl;
          return 1;
        }

        changePassword ( true, user, newPassword );
      }

      if ( program.get<bool> ( "--addCreds" ) )
      {
        if ( siteName.empty () || sitePassword.empty () || secretKey.empty () )
        {
          std::cerr << "--addCreds requires --siteName, --sitePassword and "
                       "--secretKey of your choice."
                    << std::endl;
          return 1;
        }

        addCredentials ( true, user, secretKey, siteName, sitePassword );
      }

      if ( program.get<bool> ( "--viewCreds" ) )
      {
        if ( siteName.empty () || secretKey.empty () )
        {
          std::cerr << "--viewCreds requires --siteName and --secretKey" << std::endl;
          return 1;
        }

        viewCredentials ( true, user, secretKey, siteName );
      }

      if ( program.get<bool> ( "--deleteCreds" ) )
      {
        if ( siteName.empty () )
        {
          std::cerr << "--deleteCreds requires --siteName " << std::endl;
          return 1;
        }

        deleteCredentials ( true, user, siteName );
      }

      if ( program.get<bool> ( "--changeSiteCreds" ) )
      {
        if ( siteName.empty () || sitePassword.empty () || secretKey.empty () )
        {
          std::cerr << "--changeSiteCreds requires --siteName --sitePassword (new one), and "
                       "--secretKey (new one too)";
          return 1;
        }

        changeSiteCreds ( true, user, secretKey, siteName, sitePassword );
      }
    }

    else
    {
      std::cerr << "Error please check your password. " << std::endl;
      return 1;
    }
  }

  return 0;
}
