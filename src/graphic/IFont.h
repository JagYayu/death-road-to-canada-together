#pragma once

class IFont
{
  public:
	virtual ~IFont() = default;

	// 获取原始 TTF_Font 指针
	virtual void *GetRaw() noexcept = 0;

	// 字号
	virtual int GetSize() const noexcept = 0;
};
