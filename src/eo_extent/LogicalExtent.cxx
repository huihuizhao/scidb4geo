/*
This file has been originally based on source code of SciDB (examples/example_udos/LogicalHelloInstances.cpp)
which is copyright (C) 2008-2014 SciDB, Inc.

SciDB is free software: you can redistribute it and/or modify
it under the terms of the AFFERO GNU General Public License as published by
the Free Software Foundation.

SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
the AFFERO GNU General Public License for the complete license terms.

You should have received a copy of the AFFERO GNU General Public License
along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>

-----------------------------------------------------------------------------
Modification date: (2015-08-01)

Modifications are copyright (C) 2016 Marius Appel <marius.appel@uni-muenster.de>

scidb4geo - A SciDB plugin for managing spacetime earth-observation arrays

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------*/

#include "../plugin.h" // Must be first to define PROJECT_ROOT

#include "query/Operator.h"
#include "system/Exceptions.h"
#include "array/Metadata.h"
#include "system/SystemCatalog.h"

#include "../PostgresWrapper.h"
namespace scidb4geo
{


    using namespace std;
    using namespace boost;
    using namespace scidb;

    /**
     * @brief SciDB Operator eo_bbox().
     *
     * @par Synopsis:
     *   eo_extent(nammed_array)
     * @par Summary:
     *   Computes the spatial/temporal/vertical extent of a referenced array.
     * @par Input:
     *   - named_array: an existing array that has a spatial reference
     * @par Output array:
     *   A 1 dimensional 10 attribute array with one cell.
     *
     * @par Examples:
     *   eo_extent(a);
     *
     * @par Errors:
     *   n/a
     *
     * @par Notes:
     *   Spatial coordinates are returned in the reference system, i.e. after applying the affine transformation to dimension boundaries.
     *
     *
     */
    class LogicalExtent: public LogicalOperator
    {
    public:
        LogicalExtent ( const string &logicalName, const std::string &alias ) :
            LogicalOperator ( logicalName, alias ) {
            ADD_PARAM_IN_ARRAY_NAME()
        }




        ArrayDesc inferSchema ( std::vector< ArrayDesc> inputSchemas, std::shared_ptr< Query> query ) {
            assert ( inputSchemas.size() == 0 );
            assert ( _parameters.size() == 1 );



            Attributes attributes ( 10 );
            attributes[0] = AttributeDesc ( ( AttributeID ) 0, "arrayname", TID_STRING,  0, 0 );
            attributes[1] = AttributeDesc ( ( AttributeID ) 1, "setting", TID_STRING,  0, 0 );
            attributes[2] = AttributeDesc ( ( AttributeID ) 2, "xmin", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[3] = AttributeDesc ( ( AttributeID ) 3, "xmax", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[4] = AttributeDesc ( ( AttributeID ) 4, "ymin", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[5] = AttributeDesc ( ( AttributeID ) 5, "ymax", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[6] = AttributeDesc ( ( AttributeID ) 6, "tmin", TID_STRING,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[7] = AttributeDesc ( ( AttributeID ) 7, "tmax", TID_STRING,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[8] = AttributeDesc ( ( AttributeID ) 8, "vmin", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );
            attributes[9] = AttributeDesc ( ( AttributeID ) 9, "vmax", TID_DOUBLE,  AttributeDesc::IS_NULLABLE, 0 );


            vector<DimensionDesc> dimensions ( 1 );
            dimensions[0] = DimensionDesc ( "i", 0, 0, 0, 0, 255, 0 );
            return ArrayDesc ( "Extent", attributes, dimensions, defaultPartitioning()  );
        }

    };


    REGISTER_LOGICAL_OPERATOR_FACTORY ( LogicalExtent, "eo_extent" );
    typedef LogicalExtent LogicalExtent_depr;
    REGISTER_LOGICAL_OPERATOR_FACTORY ( LogicalExtent_depr, "st_extent" ); // Backward compatibility
}

