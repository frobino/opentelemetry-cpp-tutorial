#include "opentelemetry/sdk/trace/tracer_context.h"
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/sdk/version/version.h>

namespace nostd     = opentelemetry::nostd;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace jaeger    = opentelemetry::exporter::jaeger;

opentelemetry::exporter::jaeger::JaegerExporterOptions opts;

void setUpTracer(bool inCompose, const std::string& serviceName)
{    
    // opts.server_addr = inCompose ? "jaeger" : "localhost";    

    // Create Jaeger exporter instance
    auto exporter  = std::unique_ptr<trace_sdk::SpanExporter>(new jaeger::JaegerExporter(opts));
    // Create Span Processor, needs an exporter instance to be initialized
    auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
        new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
    // Create Resource
    auto resource_attributes = opentelemetry::sdk::resource::ResourceAttributes
    {
        {"service.name", serviceName},
        {"service.instance.id", "instance-12"}
    };
    auto resource = opentelemetry::sdk::resource::Resource::Create(resource_attributes);
    // Create Sampler
    auto always_on_sampler = std::unique_ptr<trace_sdk::AlwaysOnSampler>(new trace_sdk::AlwaysOnSampler);
    // Create Tracer Context, describing the SDK configurations (Span Processors, Resource, Samplers)
    // ORIGINAL:
    // auto tracer_context = std::make_shared<trace_sdk::TracerContext>(std::move(processor), resource, std::move(always_on_sampler));

    std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> processors;
    processors.push_back(std::move(processor));
    auto tracer_context = std::make_shared<trace_sdk::TracerContext>(std::move(processors), resource, std::move(always_on_sampler));

    // Create Provider using TracerContext instance
    // NEW:
    auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(new trace_sdk::TracerProvider(tracer_context));
    // OLD
    // auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(new trace_sdk::TracerProvider(std::move(processor)));
    // Set the global trace provider
    opentelemetry::trace::Provider::SetTracerProvider(provider);
}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> getTracer(const std::string& serviceName)
{
    auto provider = opentelemetry::trace::Provider::GetTracerProvider();
    return provider->GetTracer(serviceName, OPENTELEMETRY_SDK_VERSION);
}

// frobino: this should NOT be used
void trace(std::string serviceName, const std::string& operationName)
{
    auto span = getTracer(serviceName)->StartSpan(operationName);

    span->SetAttribute("service.name", serviceName);

    // auto scope = getTracer(serviceName)->WithActiveSpan(span);
    
    // do something with span, and scope then...
    //
    span->End();
}
