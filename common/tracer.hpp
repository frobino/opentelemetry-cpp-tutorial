#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/sdk/version/version.h>

namespace nostd     = opentelemetry::nostd;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace jaeger    = opentelemetry::exporter::jaeger;

opentelemetry::exporter::jaeger::JaegerExporterOptions opts;

void setUpTracer(bool inCompose)
{    
    // opts.server_addr = inCompose ? "jaeger" : "localhost";    

    // Create Jaeger exporter instance
    auto exporter  = std::unique_ptr<trace_sdk::SpanExporter>(new jaeger::JaegerExporter(opts));
    auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
        new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
    auto provider =
    nostd::shared_ptr<opentelemetry::trace::TracerProvider>(new trace_sdk::TracerProvider(std::move(processor)));
    // Set the global trace provider
    opentelemetry::trace::Provider::SetTracerProvider(provider);
}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> getTracer(std::string serviceName)
{   
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
    return provider->GetTracer(serviceName, OPENTELEMETRY_SDK_VERSION);
}

// frobino: this should NOT be used
void trace(std::string serviceName, std::string operationName)
{
    auto span = getTracer(serviceName)->StartSpan(operationName);

    span->SetAttribute("service.name", serviceName);

    // auto scope = getTracer(serviceName)->WithActiveSpan(span);
    
    // do something with span, and scope then...
    //
    span->End();
}
