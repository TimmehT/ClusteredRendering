#include <DirectXPCH.h>
#include "Shader.h"

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
}

Shader::~Shader()
{
	Cleanup();
}

ShaderType Shader::GetType() const
{
	return m_shaderType;
}

std::string Shader::GetLatestProfile(ShaderType type)
{
	// Query the current feature level
	D3D_FEATURE_LEVEL featureLevel = m_device->GetFeatureLevel();

	switch (type)
	{
	case VertexShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "vs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "vs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "vs_4_0";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			return "vs_4_0_level_9_3";
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
			return "vs_4_0_level_9_1";
			break;
		}
		break;
	case PixelShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "ps_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "ps_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "ps_4_0";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			return "ps_4_0_level_9_3";
			break;
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
			return "ps_4_0_level_9_1";
			break;
		}
		break;
	case ComputeShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "cs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "cs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "cs_4_0";
			break;
		}
		break;
	case HullShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "hs_5_0";
			break;
		}
		break;
	case DomainShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "ds_5_0";
			break;
		}
		break;
	case GeometryShader:
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			return "gs_5_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "gs_4_1";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "gs_4_0";
			break;
		}
		break;
	}

	return "";
	
}

bool Shader::LoadShaderFromFile(ShaderType type, const std::wstring & path, const std::string & entryPoint, const std::string & profile)
{
	HRESULT hr;
	
		ID3DBlob* pShaderBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;

		std::string _profile = profile;
		if (profile == "latest")
		{
			_profile = GetLatestProfile(type);
			if (_profile.empty())
			{
				//Error
				return false;
			}
		}

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG;
#endif

		hr= D3DCompileFromFile(path.c_str(), nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), _profile.c_str(),
			flags, 0, &pShaderBlob, &pErrorBlob);

		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				//Error
				SafeRelease(pShaderBlob);
				SafeRelease(pErrorBlob);
			}
			return false;
		}

		m_shaderBlob = pShaderBlob;
	

	Cleanup();

	m_shaderType = type;

	switch (m_shaderType)
	{
	case VertexShader:
		hr = m_device->CreateVertexShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_vertexShader);
		hr = CreateInputLayoutDescription(m_shaderBlob);
		break;
	case HullShader:
		hr = m_device->CreateHullShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_hullShader);
		break;
	case DomainShader:
		hr = m_device->CreateDomainShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_domainShader);
		break;
	case GeometryShader:
		hr = m_device->CreateGeometryShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_geometryShader);
		break;
	case PixelShader:
		hr = m_device->CreatePixelShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_pixelShader);
		break;
	case ComputeShader:
		hr = m_device->CreateComputeShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_computeShader);
		break;
	default:
		//Error
		break;
	}



	SafeRelease(m_shaderBlob);
	SafeRelease(pShaderBlob);
	SafeRelease(pErrorBlob);

	if (FAILED(hr))
	{
		//Error
		return false;
	}
	
	
	return true;
}

bool Shader::LoadPreCompiledShader(ShaderType type, LPCWSTR compiledObj)
{
	HRESULT hr;

	hr = D3DReadFileToBlob(compiledObj, &m_shaderBlob);
	if (FAILED(hr))
	{
		return false;
	}

	Cleanup();

	m_shaderType = type;

	switch (m_shaderType)
	{
	case VertexShader:
		hr = m_device->CreateVertexShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_vertexShader);
		hr = CreateInputLayoutDescription(m_shaderBlob);
		break;
	case HullShader:
		hr = m_device->CreateHullShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_hullShader);
		break;
	case DomainShader:
		hr = m_device->CreateDomainShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_domainShader);
		break;
	case GeometryShader:
		hr = m_device->CreateGeometryShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_geometryShader);
		break;
	case PixelShader:
		hr = m_device->CreatePixelShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_pixelShader);
		break;
	case ComputeShader:
		hr = m_device->CreateComputeShader(m_shaderBlob->GetBufferPointer(), m_shaderBlob->GetBufferSize(), nullptr, &m_computeShader);
		break;
	default:
		//Error
		break;
	}

	SafeRelease(m_shaderBlob);

	if (FAILED(hr))
	{
		//Error
		return false;
	}

	return true;
}

void Shader::Push()
{
	switch (m_shaderType)
	{
	case VertexShader:
	{
		if (m_vertexShader)
		{
			m_context->IASetInputLayout(m_inputLayout);
			m_context->VSSetShader(m_vertexShader,nullptr,0);
		}
	}
	break;
	case HullShader:
	{
		if(m_domainShader)
		{
			m_context->HSSetShader(m_hullShader, nullptr, 0);
		}
	}
	break;
	case DomainShader:
	{
		if (m_domainShader)
		{
			m_context->DSSetShader(m_domainShader,nullptr,0);
		}
	}
	break;
	case GeometryShader:
	{
		if (m_geometryShader)
		{
			m_context->GSSetShader(m_geometryShader,nullptr,0);
		}
	}
	break;
	case PixelShader:
	{
		if (m_pixelShader)
		{
			m_context->PSSetShader(m_pixelShader,nullptr,0);
		}
	}
	break;
	case ComputeShader:
	{
		if (m_computeShader)
		{
			m_context->CSSetShader(m_computeShader,nullptr,0);
		}
	}
	break;
	}
}

void Shader::Pull()
{
	switch (m_shaderType)
	{
	case VertexShader:
	{
		if (m_vertexShader)
		{
			m_context->IASetInputLayout(nullptr);
			m_context->VSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	case HullShader:
	{
		if (m_domainShader)
		{
			m_context->HSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	case DomainShader:
	{
		if (m_domainShader)
		{
			m_context->DSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	case GeometryShader:
	{
		if (m_geometryShader)
		{
			m_context->GSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	case PixelShader:
	{
		if (m_pixelShader)
		{
			m_context->PSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	case ComputeShader:
	{
		if (m_computeShader)
		{
			m_context->CSSetShader(nullptr, nullptr,0);
		}
	}
	break;
	}
}

void Shader::Dispatch()
{
	if (m_context && m_computeShader)
	{
		//m_context->Dispatch();
	}
}

void Shader::Cleanup()
{
	SafeRelease(m_pixelShader);
	SafeRelease(m_domainShader);
	SafeRelease(m_hullShader);
	SafeRelease(m_geometryShader);
	SafeRelease(m_vertexShader);
	SafeRelease(m_computeShader);
	SafeRelease(m_inputLayout);
}

HRESULT Shader::CreateInputLayoutDescription(ID3DBlob* blob)
{
	HRESULT hr;

	// Refelect shader info
	ID3D11ShaderReflection* pShaderRefelection = nullptr;
	hr = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &pShaderRefelection);
	
	if (FAILED(hr))
	{
		return S_FALSE;
	}

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pShaderRefelection->GetDesc(&shaderDesc);

	// Read input layout desc from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pShaderRefelection->GetInputParameterDesc(i, &paramDesc);

		// load input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;
		elementDesc.Format = GetDXGIFormat(paramDesc);

		inputLayoutDesc.push_back(elementDesc);
	}

	// create input layout
	hr = m_device->CreateInputLayout(&inputLayoutDesc[0], (UINT)inputLayoutDesc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &m_inputLayout);
	
	if (FAILED(hr))
	{
		//ERROR
		return hr;
	}

	// free reflection
	SafeRelease(pShaderRefelection);

	return hr;
}

DXGI_FORMAT Shader::GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC & paramSignature)
{

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	if (paramSignature.Mask == 1)
	{
		switch (paramSignature.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			format = DXGI_FORMAT_R32_UINT;
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			format = DXGI_FORMAT_R32_SINT;
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			format = DXGI_FORMAT_R32_FLOAT;
			break;
		}
	}
	else if (paramSignature.Mask == 3)
	{
		switch (paramSignature.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			format = DXGI_FORMAT_R32G32_UINT;
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			format = DXGI_FORMAT_R32G32_SINT;
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		}
	}
	else if (paramSignature.Mask == 7)
	{
		switch (paramSignature.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			format = DXGI_FORMAT_R32G32B32_UINT;
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			format = DXGI_FORMAT_R32G32B32_SINT;
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		}
	}
	else if (paramSignature.Mask == 15)
	{
		switch (paramSignature.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			format = DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		}
	}

	return format;
}
