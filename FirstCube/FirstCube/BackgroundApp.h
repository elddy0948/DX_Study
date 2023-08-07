#pragma once

#include "D3DApp.h"

class BackgroundApp : public D3DApp
{
public:
	BackgroundApp(HINSTANCE hInstance);
	~BackgroundApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};