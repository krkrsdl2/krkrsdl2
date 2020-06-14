#include "ncbind/ncbind.hpp"

static void InitPlugin()
{
	TVPExecuteScript(TJS_W("function drawFFTGraph(){}"));
}

NCB_PRE_REGIST_CALLBACK(InitPlugin);
