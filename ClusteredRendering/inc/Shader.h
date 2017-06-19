#pragma once
enum ShaderType
{
	Unknown = 0,
	VertexShader,
	HullShader,
	DomainShader,
	GeometryShader,
	PixelShader,
	ComputeShader,
};


class Shader 
{
public:

	Shader(ID3D11Device* device, ID3D11DeviceContext* context);
	~Shader();

	ShaderType GetType()const;

	std::string GetLatestProfile(ShaderType type);

	bool LoadShaderFromFile(ShaderType type, const std::wstring& path, const std::string& entryPoint, const std::string& profile);

	bool LoadPreCompiledShader(ShaderType type, LPCWSTR compiledObj);

	void Push();

	void Pull();

	void Dispatch();

	void Cleanup();

private:

	HRESULT CreateInputLayoutDescription(ID3DBlob* blob);
	DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramSignature);

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	ShaderType m_shaderType;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11ComputeShader* m_computeShader;
	ID3D11HullShader* m_hullShader;
	ID3D11DomainShader* m_domainShader;
	ID3D11GeometryShader* m_geometryShader;

	ID3DBlob* m_shaderBlob;

	ID3D11InputLayout* m_inputLayout;

	std::string m_entryPoint;
	std::string m_profile;
	std::wstring m_shaderFilePath;


};