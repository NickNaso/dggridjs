#include <assert.h>
#include "napi.h"
#include "src/dglib.h"



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

    dp.aperture = aperture;
    dp.azimuth_deg = azimuth_deg;
    dp.pole_lat_deg = pole_lat_deg;
    dp.pole_lon_deg = pole_lon_deg;
    dp.projection = projection;
    dp.topology = topology;
    dp.res = res;

}


Napi::Value DGGSConstruct(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 7) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  // apearture. must be 3,4 or 7
  if (!info[1].IsNumber() || !info[3].IsNumber() || !info[4].IsNumber() || !info[5].IsNumber() || !info[6].IsNumber()) {
        throw Napi::Error::New(env, "Number expected for arg apearture, res, azimuth,pole lat and lon");
  }

  if(!info[0].IsString() || !info[2].IsString()){
        throw Napi::Error::New(env, "String expected");
  }

  dgconstruct(info[0].As<Napi::String>(),
    info[1].As<Napi::Number>().Int32Value(),
    info[2].As<Napi::String>(),
    info[3].As<Napi::Number>().DoubleValue(),
    info[4].As<Napi::Number>().Int32Value(),
    info[5].As<Napi::Number>().DoubleValue(),
    info[6].As<Napi::Number>().DoubleValue()
  );
  Napi::Boolean state = Napi::Boolean::New(env, true);

  return Napi::Boolean::New(env, true);
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
       std::cout << "dgconstruct(): Topology must be set! call dgconstruct() first and configure DGGS parametes.";
    }
    return false;
}



void GeoToSeqnum(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (!info[0].IsNumber() || !info[1].IsNumber()) {
        throw Napi::Error::New(env, "Number expected");
    }
    Napi::Function fn = info[2].As<Napi::Function>();
    
    double lon = info[0].As<Napi::Number>().DoubleValue();
    double lat = info[1].As<Napi::Number>().DoubleValue();

    // if (!isvalid(env)) {
    //     dgconstruct();
    // }

    dglib::Transformer dgt( 58.28252559,11.25,0,3,10,"HEXAGON","ISEA");


    std::cout << lon;
    std::cout << lat;
    auto in = dgt.inGEO(lon, lat);
    uint64_t out_seqnum;

    dgt.outSEQNUM(in, out_seqnum);
    // std::cout << out_seqnum;
  
    fn.Call(env.Global(),{ Napi::Number::New(env, out_seqnum) }); 

}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "GeoToSeqnum"), Napi::Function::New(env, GeoToSeqnum));
    exports.Set(Napi::String::New(env, "DGGSConstruct"), Napi::Function::New(env, DGGSConstruct));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
