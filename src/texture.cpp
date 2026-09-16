#include "texture.h"

Texture::Texture(const Texture&) {}
Texture::~Texture() {}
ID3D11ShaderResourceView* Texture::GetTexture() { return mTextureView; }
int Texture::GetWidth() { return mWidth; }
int Texture::GetHeight() { return mHeight; }
Texture::Texture() {
    mTargaData = nullptr;
    mTexture = nullptr;
    mTextureView = nullptr;
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename) {
    bool result;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT hResult;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    result = LoadTarga32Bit(filename);
    if (!result) return false;

	textureDesc.Height = mHeight;
	textureDesc.Width = mWidth;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    
	// textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	// textureDesc.CPUAccessFlags = 0;
	// textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    
    hResult = device->CreateTexture2D(&textureDesc, NULL, &mTexture);
    if (FAILED(hResult)) return false;

    rowPitch = (mWidth * 4) * sizeof(unsigned char);
    deviceContext->UpdateSubresource(mTexture, 0, NULL, mTargaData, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

    hResult = device->CreateShaderResourceView(mTexture, &srvDesc, &mTextureView);
    if (FAILED(hResult)) return false;
    deviceContext->GenerateMips(mTextureView);
    
    delete [] mTargaData;
    mTargaData = nullptr;

    return true;
}

void Texture::Shutdown() {
    if (mTextureView) {
        mTextureView->Release();
        mTextureView = nullptr;
    }
    if (mTexture) {
        mTexture->Release();
        mTexture = nullptr;
    }
    if (mTargaData) {
        delete [] mTargaData;
        mTargaData = nullptr;
    }
}

bool Texture::LoadTarga32Bit(const char* filename) {
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	mHeight = (int)targaFileHeader.height;
	mWidth = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = mWidth * mHeight * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	mTargaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (mWidth * mHeight * 4) - (mWidth * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for(j=0; j<mHeight; j++)
	{
		for(i=0; i<mWidth; i++)
		{
			mTargaData[index + 0] = targaImage[k + 2];  // Red.
			mTargaData[index + 1] = targaImage[k + 1];  // Green.
			mTargaData[index + 2] = targaImage[k + 0];  // Blue
			mTargaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (mWidth * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete [] targaImage;
	targaImage = 0;

	return true;
    
    // int error, bpp, imageSize, index;
    // FILE* filePtr;
    // unsigned int count;
    // TargaHeader targaFileHeader;
    // unsigned char* targaImage;

    // error = fopen_s(&filePtr, filename, "rb");
    // if (error != 0) return false;

    // count = (unsigned int) fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    // if (count != 1) {
    //     fclose(filePtr);
    //     return false;
    // }

    // mHeight = (int)targaFileHeader.height;
    // mWidth = (int)targaFileHeader.width;
    // bpp = (int)targaFileHeader.bpp;
    // if (bpp != 32) {
    //     fclose(filePtr);
    //      return false;
    // }

    // imageSize = mWidth * mHeight * 4;
    // targaImage = new unsigned char[imageSize];

    // count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    // if (count != imageSize) {
    //     fclose(filePtr);
    //      return false;
    // }

    // error = fclose(filePtr);
    // if (error != 0) return false;

    // mTargaData = new unsigned char[imageSize];
    // index = 0;

    // int k = (mWidth * mHeight * 4) - (mWidth * 4);
    // for (int j = 0; j < mHeight; j++) {
    //     for (int i = 0; i < mWidth; i++) {
    //         mTargaData[index + 0] = targaImage[k + 2];
    //         mTargaData[index + 1] = targaImage[k + 1];
    //         mTargaData[index + 2] = targaImage[k + 0];
    //         mTargaData[index + 3] = targaImage[k + 3];
    //         k += 4;
    //         index += 4;
    //     }
    //     k -= (mWidth * 8);
    // }
    // delete [] targaImage;
    // targaImage = nullptr;

    // return true;
}
