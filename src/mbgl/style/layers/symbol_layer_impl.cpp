#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void SymbolLayer::Impl::cascade(const CascadeParameters& parameters) {
    iconPaint.cascade(parameters);
    textPaint.cascade(parameters);
}

bool SymbolLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    iconPaint.evaluate(parameters);
    textPaint.evaluate(parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    iconSize = layout.evaluate<IconSize>(parameters);
    textSize = layout.evaluate<TextSize>(parameters);

    passes = ((iconPaint.evaluated.get<SymbolOpacity>() > 0 && (iconPaint.evaluated.get<SymbolColor>().a > 0 || iconPaint.evaluated.get<SymbolHaloColor>().a > 0) && iconSize > 0)
           || (textPaint.evaluated.get<SymbolOpacity>() > 0 && (textPaint.evaluated.get<SymbolColor>().a > 0 || textPaint.evaluated.get<SymbolHaloColor>().a > 0) && textSize > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return iconPaint.hasTransition() || textPaint.hasTransition();
}

std::unique_ptr<Bucket> SymbolLayer::Impl::createBucket(const BucketParameters&, const std::vector<const Layer*>&) const {
    assert(false); // Should be calling createLayout() instead.
    return nullptr;
}

std::unique_ptr<SymbolLayout> SymbolLayer::Impl::createLayout(const BucketParameters& parameters,
                                                              const std::vector<const Layer*>& group,
                                                              const GeometryTileLayer& layer) const {
    return std::make_unique<SymbolLayout>(parameters,
                                          group,
                                          layer,
                                          *spriteAtlas);
}

SymbolPropertyValues SymbolLayer::Impl::iconPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<IconRotationAlignment>(), // icon-pitch-alignment is not yet implemented; inherit the rotation alignment
        layout_.get<IconRotationAlignment>(),
        layout_.get<IconSize>(),
        iconPaint.evaluated.get<SymbolOpacity>(),
        iconPaint.evaluated.get<SymbolColor>(),
        iconPaint.evaluated.get<SymbolHaloColor>(),
        iconPaint.evaluated.get<SymbolHaloWidth>(),
        iconPaint.evaluated.get<SymbolHaloBlur>(),
        iconPaint.evaluated.get<SymbolTranslate>(),
        iconPaint.evaluated.get<SymbolTranslateAnchor>(),
        iconSize,
        1.0f
    };
}

SymbolPropertyValues SymbolLayer::Impl::textPropertyValues(const SymbolLayoutProperties::Evaluated& layout_) const {
    return SymbolPropertyValues {
        layout_.get<TextPitchAlignment>(),
        layout_.get<TextRotationAlignment>(),
        layout_.get<TextSize>(),
        textPaint.evaluated.get<SymbolOpacity>(),
        textPaint.evaluated.get<SymbolColor>(),
        textPaint.evaluated.get<SymbolHaloColor>(),
        textPaint.evaluated.get<SymbolHaloWidth>(),
        textPaint.evaluated.get<SymbolHaloBlur>(),
        textPaint.evaluated.get<SymbolTranslate>(),
        textPaint.evaluated.get<SymbolTranslateAnchor>(),
        textSize,
        24.0f
    };
}

} // namespace style
} // namespace mbgl
