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

#include "../plugin.h"  // Must be first to define PROJECT_ROOT

#include "array/Metadata.h"
#include "query/Operator.h"
#include "system/Exceptions.h"
#include "system/SystemCatalog.h"

#include "../PostgresWrapper.h"
namespace scidb4geo {

    using namespace std;
    using namespace boost;
    using namespace scidb;

    //TODO: Update docs

    /**
    * @brief SciDB Operator eo_gettrs().
    *
    * @par Synopsis:
    *   eo_gettrs(nammed_array1, [named_array2, ...])
    * @par Summary:
    *   Returns spatial reference information of given arrays
    * @par Input:
    *   - named_array1: An existing array that has a spatial reference
    * @par Output array:
    *   One tuple with attributes (name, xdim, ydim, auth_name, auth_id, srtext, proj4text, A) per array.
    *
    * @par Examples:
    *   eo_gettrs(a,b,c);
    *
    * @par Errors:
    *   n/a
    *
    * @par Notes:
    *   The number of returned rows might be less than provided arrays, only already registered arrays will be returned
    *
    *
    */
    class LogicalGetTRS : public LogicalOperator {
       public:
        LogicalGetTRS(const string &logicalName, const std::string &alias) : LogicalOperator(logicalName, alias) {
            ADD_PARAM_VARIES()  // Expect a variable list of parameters (all named arrays)
        }

        vector<std::shared_ptr<OperatorParamPlaceholder> > nextVaryParamPlaceholder(const vector<ArrayDesc> &schemas) {
            vector<std::shared_ptr<OperatorParamPlaceholder> > res;
            res.push_back(PARAM_IN_ARRAY_NAME());
            res.push_back(END_OF_VARIES_PARAMS());
            return res;
        }

        ArrayDesc inferSchema(std::vector<ArrayDesc> inputSchemas, std::shared_ptr<Query> query) {
            assert(inputSchemas.size() == 0);

            Attributes attributes(6);
            attributes[0] = AttributeDesc((AttributeID)0, "name", TID_STRING, 0, 0);
            attributes[1] = AttributeDesc((AttributeID)1, "tdim", TID_STRING, 0, 0);
            attributes[2] = AttributeDesc((AttributeID)2, "t0", TID_STRING, 0, 0);
            attributes[3] = AttributeDesc((AttributeID)3, "dt", TID_STRING, 0, 0);
            attributes[4] = AttributeDesc((AttributeID)4, "tMin", TID_STRING, 0, 0);
            attributes[5] = AttributeDesc((AttributeID)5, "tMax", TID_STRING, 0, 0);

            size_t nArrays = _parameters.size();
            if (nArrays == 0) nArrays = (size_t)PostgresWrapper::instance()->dbGetTemporalRefCount();
            size_t end = nArrays > 0 ? nArrays - 1 : 0;
            vector<DimensionDesc> dimensions(1);

            dimensions[0] = DimensionDesc("i", 0, 0, end, end, nArrays, 0);

            stringstream ss;
            ss << query->getInstanceID();
            ArrayDistPtr localDist = ArrayDistributionFactory::getInstance()->construct(psLocalInstance, DEFAULT_REDUNDANCY, ss.str());
            return ArrayDesc("Temporal Array", attributes, dimensions, localDist, query->getDefaultArrayResidency());
        }
    };
    REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalGetTRS, "eo_gettrs");
    typedef LogicalGetTRS LogicalGetTRS_depr;
    REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalGetTRS_depr, "st_gettrs");  // Backward compatibility
}
