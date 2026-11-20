#include "ShaderImageUI.h"

#include "../Renderer/PixelMaterial.h"
#include "../Renderer/PixelRenderer.h"

ShaderImageUI::ShaderImageUI(Vector2 pos, Vector2 size, const std::wstring& resourceKey)
	:
	size_(size),
	ImageUI(pos, resourceKey)
{
}

void ShaderImageUI::Init(void)
{
	ImageUI::Init();

	material_ = std::make_unique<PixelMaterial>(L"Texture.cso", 0);
	material_->AddTextureBuf(handle_);
	renderer_ = std::make_unique<PixelRenderer>(*material_);
	renderer_->MakeSquereVertex(
		Vector2(pos_.x, pos_.y),
		Vector2(size_.x, size_.y)
	);
}

void ShaderImageUI::Draw(void) const
{
	renderer_->Draw();
}
