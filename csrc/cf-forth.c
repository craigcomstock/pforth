#include "cf3.defs.h"
#include "pforth.h"
#include "pf_all.h"
#include "generic_agent.h"  /* GenericAgentConfig */

extern void EvalContextStackPushPromiseTypeFrame(EvalContext *ctx, const PromiseType *owner);
extern void KeepPromises(EvalContext *ctx, const Policy *policy, GenericAgentConfig *config);
extern void EvalContextStackPushBundleFrame(EvalContext *ctx, const Bundle *owner, const Rlist *args, bool inherits_previous);
extern Policy *g_policy;
extern EvalContext *g_ctx;
extern GenericAgentConfig *g_config;
extern Promise *g_promise;
extern PromiseType *g_promise_type;
extern Bundle *g_bundle;
// cf-forth C function hooks
static cell_t CPromise( cell_t Val )
{
  MSG_NUM_D("CPromise: Val = ", Val);
  KeepPromises(g_ctx, g_policy, g_config);
  return Val+1;
}
// Bundle *bp = PolicyAppendBundle( policy, "default", "none", "agent", NULL, NULL);
// PolicyDestroy(policy)
// see core/tests/unit/expand_test.c:395 test_expand_promise_array_with_scalar_arg()
// maybe BundleToString(Writer *writer, Bundle *bundle)
// nay, PolicyToString(const Policy *policy, Writer *writer)
static cell_t CPolicyString( cell_t Val )
{
  MSG_NUM_D("CPolicyString: Val = ", Val);
  Writer *w = StringWriter();
  PolicyToString(g_policy, w);
  printf("%s\n", StringWriterData(w));
  WriterClose(w);
  return Val+1;
}
static cell_t CPolicyNew( cell_t Val ) {
MSG_NUM_D("CPolicyNew: Val = ", Val);
g_policy = PolicyNew();
Bundle *bundle = PolicyAppendBundle(g_policy, NamespaceDefault(), "main", "agent", NULL, NULL);
PromiseType *promise_type = BundleAppendPromiseType(bundle, "reports");
Promise *promise = PromiseTypeAppendPromise(promise_type, "hi", (Rval) { NULL, RVAL_TYPE_NOPROMISEE }, "any", NULL);
printf("promise = %p\n", promise); // just to use the variable :p
// what if I switched these two? push promisetypeframe THEN bundleframe?
EvalContextStackPushBundleFrame(g_ctx, bundle, NULL, false);
EvalContextStackPushPromiseTypeFrame(g_ctx, promise_type);
return Val+1;

//ExpandPromise(g_ctx, promise, acutator_expand_promise_array_with_scalar_arg, NULL); // TODO, certainly wrong yeah?
//EvalContextStackPopFrame(g_ctx);
//EvalContextStackPopFrame(g_ctx);

}

// TODO, make this conditional somehow on FORTH being defined as an option?
CFunc0 CustomFunctionTable[] = 
{
  (CFunc0) CPromise,
  (CFunc0) CPolicyString,
  (CFunc0) CPolicyNew
};
Err CompileCustomFunctions( void )
{
printf("CompileCustomFunctions()\n");
  Err err;
  int i = 0;
  err = CreateGlueToC( "PROMISE", i++, C_RETURNS_VALUE, 1);
  if (err < 0 ) return err;
  err = CreateGlueToC( "POLICYSTRING", i++, C_RETURNS_VALUE, 1);
  if (err < 0) return err;
  err = CreateGlueToC( "POLICYNEW", i++, C_RETURNS_VALUE, 1);
  if (err < 0) return err;
  return 0;
}
