#include <assert.h>
#include <node_api.h>
#include "src/dglib.h"
#include "src/DgParams.h"



struct DgParam
{
  long double  pole_lon_deg;
  long double  pole_lat_deg;
  long double  azimuth_deg;
  unsigned int aperture;
  int          res;
  std::string  topology = "cheese";    //"HEXAGON", "DIAMOND", "TRIANGLE"
  std::string  projection;  //ISEA/FULLER
};
DgParam dp;

/*
 * Construct a DGGS
 */
void dgconstruct(string projection = "ISEA", int aperture = 3,
                 string topology = "HEXAGON",
                 double azimuth_deg = 0,
                 double res = 10,
                 double pole_lon_deg = 11.25, double pole_lat_deg = 58.28252559) {

    std::cout << "** executing DGGRID version " << DGGRID_VERSION << " **\n";
    std::cout << "type sizes: big int: " << sizeof(long long int) * 8 << " bits / ";
    std::cout << "big double: " << sizeof(long double) * 8 << " bits\n";

    dp.aperture = aperture;
    dp.azimuth_deg = azimuth_deg;
    dp.pole_lat_deg = pole_lat_deg;
    dp.pole_lon_deg = pole_lon_deg;
    dp.projection = projection;
    dp.topology = topology;
    dp.res = res;

}
napi_value dggsConstruct(napi_env env, napi_callback_info info) {
  dgconstruct();
  return NULL;
}
/*
 * Helper function to check if an item is in a vector or not
 */
bool in_array(const std::string &value, const std::vector<string> &array) {
    return std::find(array.begin(), array.end(), value) != array.end();
}


/*
 * checks if DgParams are valid or not
 * FIXME: only topology is checked here
 */
bool isvalid(napi_env env) {
    std::vector<std::string> topologies{"HEXAGON", "DIAMOND", "TRIANGLE"};

    if (in_array(dp.topology, topologies)) {
        return true;
    } else {
      napi_throw_error(env,0, "dgconstruct(): Topology must be set! call dgconstruct() first and configure DGGS parametes.");
      // throw Napi::Error::New(env, "dgconstruct(): Topology must be set! call dgconstruct() first and configure DGGS parametes.");
      // Error::New(env, "Example exception").ThrowAsJavaScriptException();
    }
    return false;
}

// /*
//  * Convert a lat/long point into a sequence number
//  */
// uint64_t geo_to_seq(double in_lon_deg, double in_lat_deg) {

//     if (!isvalid()) {
//         dgconstruct();
//     }

//     dglib::Transformer dgt(dp);

//     auto in = dgt.inGEO(in_lon_deg, in_lat_deg);
//     uint64_t out_seqnum;

//     dgt.outSEQNUM(in, out_seqnum);
//     return out_seqnum;

// }


napi_value GeoToSeqnum(napi_env env, const napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value argv[1];



  status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
  assert(status == napi_ok);

  napi_value cb = argv[2];



    // if (!argv[0].IsNumber() || !argv[1].IsNumber() ){
    //       napi_throw_error(env,0, "Number expected");
    // } 



    if (!isvalid(env)) {
        dgconstruct();
    }

    dglib::DgParams dgp;
    dgp.aperture=dp.aperture;
    dgp.azimuth_deg = dp.azimuth_deg;
    dgp.res = dp.res;
    dgp.pole_lat_deg = dp.pole_lat_deg;
    dgp.pole_lon_deg = dp.pole_lon_deg;
    dgp.topology = dp.topology;
    dgp.projection = dp.projection;

    dglib::Transformer dgt(dgp);

    double lon;
    double lat;
    status = napi_get_value_double(env,argv[0],&lon);
    assert(status == napi_ok);

    status = napi_get_value_double(env,argv[1],&lat);
    assert(status == napi_ok);

    auto in = dgt.inGEO(lon, lat);
    uint64_t out_seqnum;

    dgt.outSEQNUM(in, out_seqnum);


  napi_value seqnum[1];
  status = napi_create_int64(env,out_seqnum,  seqnum);
  assert(status == napi_ok);

  napi_value global;
  status = napi_get_global(env, &global);
  assert(status == napi_ok);

  napi_value result;
  status = napi_call_function(env, global, cb, 1, seqnum, &result);
  assert(status == napi_ok);

  return nullptr;
}





napi_value Init(napi_env env, napi_value exports) {
  napi_status status;

  napi_value fn;
  status = napi_create_function(env, NULL, 0, dggsConstruct, NULL, &fn);
  if (status != napi_ok) return NULL;

  status = napi_set_named_property(env, exports, "DGGSConstruct", fn);
  if (status != napi_ok) return NULL;

 
  
  status = napi_create_function(
      env, "", NAPI_AUTO_LENGTH, GeoToSeqnum, nullptr, &fn);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "GeoToSeqnum", fn);
 
   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
