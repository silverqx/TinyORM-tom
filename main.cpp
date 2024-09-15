#include <orm/db.hpp>
#include <orm/utils/configuration.hpp>

#include <tom/application.hpp>

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

#include "seeders/databaseseeder.hpp"

using Orm::DatabaseManager;
using Orm::DB;

using ConfigUtils = Orm::Utils::Configuration;

using TomApplication = Tom::Application;

using namespace Migrations; // NOLINT(google-build-using-namespace)
using namespace Seeders;    // NOLINT(google-build-using-namespace)

/*! Create the database manager instance and add a database connection. */
static std::shared_ptr<DatabaseManager> setupDatabaseManager(); // NOLINT(misc-use-anonymous-namespace)

/*! C++ main function. */
int main(int argc, char *argv[])
{
    try {
        // Ownership of the shared_ptr()
        auto db = setupDatabaseManager();

        return TomApplication(argc, argv, std::move(db), "TOM_EXAMPLE_ENV")
                .migrations<CreatePostsTable,
                            AddFactorColumnToPostsTable,
                            CreatePropertiesTable,
                            CreatePhonesTable>()
                .seeders<DatabaseSeeder>()
                // Fire it up 🔥🚀✨
                .run();

    } catch (const std::exception &e) {

        TomApplication::logException(e);
    }

    return EXIT_FAILURE;
}

std::shared_ptr<DatabaseManager> setupDatabaseManager()
{
    using namespace Orm::Constants; // NOLINT(google-build-using-namespace)

    // Ownership of the shared_ptr()
    return DB::create({
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MYSQL_HOST",      H127001)},
        {port_,           qEnvironmentVariable("DB_MYSQL_PORT",      P3306)},
        {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE",  EMPTY)},
        {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME",  EMPTY)},
        {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD",  EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET",   UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        // SYSTEM - set the time zone to your local MySQL server time zone
        {timezone_,       TZ00},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the QTimeZone::UTC, set to the QTimeZone::LocalTime or
           QtTimeZoneType::DontConvert to use the system local time. */
        {qt_timezone,     QVariant::fromValue(QTimeZone::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, u"REPEATABLE READ"_s}, // MySQL default is REPEATABLE READ for InnoDB
        {engine_,         InnoDB},
        {options_,        ConfigUtils::mysqlSslOptions()},
    },
        u"tinyorm_tom_mysql"_s); // shell:connection
}
