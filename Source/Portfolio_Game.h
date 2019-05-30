#pragma once

#include "d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

class Textures;
class Materials;
class Player;
class Monster;

class PortfolioGameApp : public D3DApp
{
public:
	PortfolioGameApp(HINSTANCE hInstance);
	PortfolioGameApp(const PortfolioGameApp& rhs) = delete;
	PortfolioGameApp& operator=(const PortfolioGameApp& rhs) = delete;
	~PortfolioGameApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
	void OnKeyboardInput(const GameTimer& gt);
	void checkCollision(
		const std::vector<RenderItem*>& rItems,
		const BoundingBox &playerBoundForward,
		const BoundingBox &playerBoundBackward,
		bool &isForward,
		bool &isBackward,
		const float &dt);

	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateMaterialCB(const GameTimer& gt);
	void UpdateCharacterCBs(const GameTimer & gt);
	void UpdateObjectShadows(const GameTimer & gt);

	void LoadTextures();
	void BuildDescriptorHeaps();
	void BuildTextureBufferViews();
	void BuildConstantBufferViews();
	void BuildRootSignature();
	void BuildPostProcessRootSignature();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildArcheGeometry(
		const std::vector<std::vector<Vertex>>& outVertices,
		const std::vector<std::vector<std::uint32_t>>& outIndices,
		const std::vector<std::string>& geoName);

	void BuildFbxGeometry();
	void LoadFBXPlayer();
	void BuildFBXTexture(
		std::vector<Material> &outMaterial, 
		std::string inTextureName, std::string inMaterialName);
	void LoadFBXMonster();
	void LoadFBXSubMonster(
		std::vector<Material> &outMaterial, 
		std::string& inMaterialName, std::string &FileName, 
		bool isEvenX, bool isEvenZ);
	void LoadFBXArchitecture();

	void BuildMaterials();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void BuildLandscapeRitems(UINT& objCBIndex);
	void BuildSubRitems(
		std::string geoName,
		std::string subRitemName,
		std::string matName,
		RenderLayer subRtype,
		UINT &objCBIndex,
		FXMMATRIX& worldTransform,
		CXMMATRIX& texTransform);
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12RootSignature> mPostProcessRootSignature = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mSkinnedInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mUIInputLayout;
	
	// Pass
	PassConstants mMainPassCB;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitems[(int)RenderLayer::Count];

	// Skill Icon time
	float HitTime[(int)eUIList::Count];
	float DelayTime[(int)eUIList::Count];

	UINT mObjCbvOffset = 0;
	UINT mChaCbvOffset = 0;
	UINT mMonsterCbvOffset = 0;
	UINT mPassCbvOffset = 0;
	UINT mMatCbvOffset = 0;
	UINT mUICbvOffset = 0;
	UINT mMonsterUICbvOffset = 0;
	UINT mTextureOffset = 0;

	UINT mCbvSrvDescriptorSize = 0;

	bool mIsWireframe = false;
	bool mFbxWireframe = false;
	bool mCameraDetach = false; // True - Camera Move with player

	Light mMainLight;

	POINT mLastMousePos;

	Player mPlayer;
	Monster* mMonster;
	std::vector<std::unique_ptr<Monster>> mMonstersByZone;
	UINT mZoneIndex;

	Textures mTextures;
	Materials mMaterials;

	std::unique_ptr<BlurCOC> mBlurCOC;

};