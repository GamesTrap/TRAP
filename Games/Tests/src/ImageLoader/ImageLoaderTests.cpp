#include "ImageLoaderTests.h"

ImageLoaderTests::ImageLoaderTests()
	: Layer("ImageLoader")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void ImageLoaderTests::OnImGuiRender()
{
	ImGui::Begin("Image loader", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                      ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Keys: 1(TGA), 2(PNG), 3(BMP), 4(PM), 5(Radiance), 6(QOI)");
	if(m_tga)
	{
		ImGui::Text("Current: TGA");
		ImGui::Text("1. Test16BPP");
		ImGui::Text("2. Test16BPPRLE");
		ImGui::Text("3. Test24BPP");
		ImGui::Text("4. Test24BPPRLE");
		ImGui::Text("5. Test32BPP");
		ImGui::Text("6. Test32BPPRLE");
		ImGui::Text("7. TestGrayscale");
		ImGui::Text("8. TestPalette");
	}
	else if(m_png)
	{
		ImGui::Text("Current: PNG");
		ImGui::Text(" 1. Test8BPPGrayscaleBig");
		ImGui::Text(" 2. Test8BPPGrayscaleBigInterlaced");
		ImGui::Text(" 3. Test8BPPGrayscaleSmaller");
		ImGui::Text(" 4. Test8BPPGrayscaleSmallest");
		ImGui::Text(" 5. Test16BPPGrayscaleBig");
		ImGui::Text(" 6. Test16BPPGrayscaleBigInterlaced");
		ImGui::Text(" 7. Test16BPPGrayscaleSmaller");
		ImGui::Text(" 8. Test16BPPGrayscaleSmallest");
		ImGui::Text(" 9. Test24BPPBig");
		ImGui::Text("10. Test24BPPBigInterlaced");
		ImGui::Text("11. Test24BPPSmaller");
		ImGui::Text("12. Test24BPPSmallest");
		ImGui::Text("13. Test48BPPBig");
		ImGui::Text("14. Test48BPPBigInterlaced");
		ImGui::Text("15. Test48BPPSmaller");
		ImGui::Text("16. Test48BPPSmallest");
		ImGui::Text("17. TestPaletteBig");
		ImGui::Text("18. TestPaletteBigInterlaced");
		ImGui::Text("19. TestPaletteSmaller");
		ImGui::Text("20. TestPaletteSmallest");
	}
	else if(m_bmp)
	{
		ImGui::Text("Current: BMP");
		ImGui::Text(" 1. Test8BPPGrayscale");
		ImGui::Text(" 2. Test8BPPGrayscaleReversed");
		ImGui::Text(" 3. Test8BPPGrayscaleRLE");
		ImGui::Text(" 4. Test8BPPGrayscaleRLEReversed");
		ImGui::Text(" 5. Test8BPPPalette");
		ImGui::Text(" 6. Test8BPPPaletteReversed");
		ImGui::Text(" 7. Test8BPPPaletteRLE");
		ImGui::Text(" 8. Test8BPPPaletteRLEReversed");
		ImGui::Text(" 9. Test16BPP");
		ImGui::Text("10. Test16BPPReversed");
		ImGui::Text("11. Test16BPPBitFields");
		ImGui::Text("12. Test24BPP");
		ImGui::Text("13. Test24BPPReversed");
		ImGui::Text("14. Test32BPP");
		ImGui::Text("15. Test32BPPReversed");
		ImGui::Text("16. Test32BPPBitFields");
	}
	else if(m_pm)
	{
		ImGui::Text("Current: PM");
		ImGui::Text("1. Test8BPPGrayscale (PGM)");
		ImGui::Text("2. Test8BPPGrayscale (PAM)");
		ImGui::Text("3. Test16BPPGrayscale (PGM)");
		ImGui::Text("4. Test16BPPGrayscale (PAM)");
		ImGui::Text("5. Test24BPP (PPM)");
		ImGui::Text("6. Test24BPP (PAM)");
		ImGui::Text("7. Test48BPP (PPM)");
		ImGui::Text("8. Test48BPP (PAM)");
		ImGui::Text("9. TestGrayscaleHDR (PFM)");
		ImGui::Text("10. TestHDR (PFM)");
	}
	else if(m_radiance)
	{
		ImGui::Text("Current: Radiance");
		ImGui::Text("1. TestHDR");
	}
	else if(m_qoi)
	{
		ImGui::Text("Current: Quite OK Image");
		ImGui::Text("1. Test24BPPsRGB");
		ImGui::Text("2. Test24BPPLinear");
		ImGui::Text("3. Test32BPPsRGB");
		ImGui::Text("4. Test32BPPLinear");
	}
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImageLoaderTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("ImageLoader");
	TRAP::Application::SetHotReloading(true);

	//Load Textures
	const TRAP::Utils::Timer loadingTimer;
	//TGA
	TRAP::Graphics::TextureManager::Load("TGATest16BPP", "./Assets/Textures/TGA/Test16BPP.tga");
	TRAP::Graphics::TextureManager::Load("TGATest16BPPRLE", "./Assets/Textures/TGA/Test16BPPRLE.tga");
	TRAP::Graphics::TextureManager::Load("TGATest24BPP", "./Assets/Textures/TGA/Test24BPP.tga");
	TRAP::Graphics::TextureManager::Load("TGATest24BPPRLE", "./Assets/Textures/TGA/Test24BPPRLE.tga");
	TRAP::Graphics::TextureManager::Load("TGATest32BPP", "./Assets/Textures/TGA/Test32BPP.tga");
	TRAP::Graphics::TextureManager::Load("TGATest32BPPRLE", "./Assets/Textures/TGA/Test32BPPRLE.tga");
	TRAP::Graphics::TextureManager::Load("TGATestGrayscale", "./Assets/Textures/TGA/TestGrayscale.tga");
	TRAP::Graphics::TextureManager::Load("TGATestPalette", "./Assets/Textures/TGA/TestPalette.tga");

	//BMP
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPGrayscale", "./Assets/Textures/BMP/Test8BPPGrayscale.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPGrayscaleReversed",
	                                     "./Assets/Textures/BMP/Test8BPPGrayscaleReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPGrayscaleRLE", "./Assets/Textures/BMP/Test8BPPGrayscaleRLE.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPGrayscaleRLEReversed",
	                                     "./Assets/Textures/BMP/Test8BPPGrayscaleRLEReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPPalette", "./Assets/Textures/BMP/Test8BPPPalette.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPPaletteReversed", "./Assets/Textures/BMP/Test8BPPPaletteReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPPaletteRLE", "./Assets/Textures/BMP/Test8BPPPaletteRLE.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest8BPPPaletteRLEReversed",
	                                     "./Assets/Textures/BMP/Test8BPPPaletteRLEReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest16BPP", "./Assets/Textures/BMP/Test16BPP.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest16BPPReversed", "./Assets/Textures/BMP/Test16BPPReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest16BPPBitFields", "./Assets/Textures/BMP/Test16BPPBitFields.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest24BPP", "./Assets/Textures/BMP/Test24BPP.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest24BPPReversed", "./Assets/Textures/BMP/Test24BPPReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest32BPP", "./Assets/Textures/BMP/Test32BPP.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest32BPPReversed", "./Assets/Textures/BMP/Test32BPPReversed.bmp");
	TRAP::Graphics::TextureManager::Load("BMPTest32BPPBitFields", "./Assets/Textures/BMP/Test32BPPBitFields.bmp");

	//PNG
	TRAP::Graphics::TextureManager::Load("PNGTest8BPPGrayscaleBig", "./Assets/Textures/PNG/Test8BPPGrayscaleBig.png");
	TRAP::Graphics::TextureManager::Load("PNGTest8BPPGrayscaleBigInterlaced",
	                                     "./Assets/Textures/PNG/Test8BPPGrayscaleBigInterlaced.png");
	TRAP::Graphics::TextureManager::Load("PNGTest8BPPGrayscaleSmaller",
	                                     "./Assets/Textures/PNG/Test8BPPGrayscaleSmaller.png");
	TRAP::Graphics::TextureManager::Load("PNGTest8BPPGrayscaleSmallest",
	                                     "./Assets/Textures/PNG/Test8BPPGrayscaleSmallest.png");
	TRAP::Graphics::TextureManager::Load("PNGTest16BPPGrayscaleBig", "./Assets/Textures/PNG/Test16BPPGrayscaleBig.png");
	TRAP::Graphics::TextureManager::Load("PNGTest16BPPGrayscaleBigInterlaced",
	                                     "./Assets/Textures/PNG/Test16BPPGrayscaleBigInterlaced.png");
	TRAP::Graphics::TextureManager::Load("PNGTest16BPPGrayscaleSmaller",
	                                     "./Assets/Textures/PNG/Test16BPPGrayscaleSmaller.png");
	TRAP::Graphics::TextureManager::Load("PNGTest16BPPGrayscaleSmallest",
	                                     "./Assets/Textures/PNG/Test16BPPGrayscaleSmallest.png");
	TRAP::Graphics::TextureManager::Load("PNGTest24BPPBig", "./Assets/Textures/PNG/Test24BPPBig.png");
	TRAP::Graphics::TextureManager::Load("PNGTest24BPPBigInterlaced", "./Assets/Textures/PNG/Test24BPPBigInterlaced.png");
	TRAP::Graphics::TextureManager::Load("PNGTest24BPPSmaller", "./Assets/Textures/PNG/Test24BPPSmaller.png");
	TRAP::Graphics::TextureManager::Load("PNGTest24BPPSmallest", "./Assets/Textures/PNG/Test24BPPSmallest.png");
	TRAP::Graphics::TextureManager::Load("PNGTest48BPPBig", "./Assets/Textures/PNG/Test48BPPBig.png");
	TRAP::Graphics::TextureManager::Load("PNGTest48BPPBigInterlaced", "./Assets/Textures/PNG/Test48BPPBigInterlaced.png");
	TRAP::Graphics::TextureManager::Load("PNGTest48BPPSmaller", "./Assets/Textures/PNG/Test48BPPSmaller.png");
	TRAP::Graphics::TextureManager::Load("PNGTest48BPPSmallest", "./Assets/Textures/PNG/Test48BPPSmallest.png");
	TRAP::Graphics::TextureManager::Load("PNGTestPaletteBig", "./Assets/Textures/PNG/TestPaletteBig.png");
	TRAP::Graphics::TextureManager::Load("PNGTestPaletteBigInterlaced",
	                                     "./Assets/Textures/PNG/TestPaletteBigInterlaced.png");
	TRAP::Graphics::TextureManager::Load("PNGTestPaletteSmaller", "./Assets/Textures/PNG/TestPaletteSmaller.png");
	TRAP::Graphics::TextureManager::Load("PNGTestPaletteSmallest", "./Assets/Textures/PNG/TestPaletteSmallest.png");

	//PM
	TRAP::Graphics::TextureManager::Load("PMTest8BPPGrayscalePGM", "./Assets/Textures/PM/Test8BPPGrayscale.pgm");
	TRAP::Graphics::TextureManager::Load("PMTest8BPPGrayscalePAM", "./Assets/Textures/PM/Test8BPPGrayscale.pam");
	TRAP::Graphics::TextureManager::Load("PMTest16BPPGrayscalePGM", "./Assets/Textures/PM/Test16BPPGrayscale.pgm");
	TRAP::Graphics::TextureManager::Load("PMTest16BPPGrayscalePAM", "./Assets/Textures/PM/Test16BPPGrayscale.pam");
	TRAP::Graphics::TextureManager::Load("PMTest24BPPPPM", "./Assets/Textures/PM/Test24BPP.ppm");
	TRAP::Graphics::TextureManager::Load("PMTest24BPPPAM", "./Assets/Textures/PM/Test24BPP.pam");
	TRAP::Graphics::TextureManager::Load("PMTest48BPPPPM", "./Assets/Textures/PM/Test48BPP.ppm");
	TRAP::Graphics::TextureManager::Load("PMTest48BPPPAM", "./Assets/Textures/PM/Test48BPP.pam");
	TRAP::Graphics::TextureManager::Load("PMTestGrayscaleHDRPFM", "./Assets/Textures/PM/TestGrayscaleHDR.pfm");
	TRAP::Graphics::TextureManager::Load("PMTestHDRPFM", "./Assets/Textures/PM/TestHDR.pfm");

	//Radiance
	TRAP::Graphics::TextureManager::Load("RadianceTestHDR", "./Assets/Textures/Radiance/TestHDR.hdr");

	//QOI
	TRAP::Graphics::TextureManager::Load("QOITest24BPPsRGB", "./Assets/Textures/QOI/Test24BPPsRGB.qoi");
	TRAP::Graphics::TextureManager::Load("QOITest24BPPLinear", "./Assets/Textures/QOI/Test24BPPLinear.qoi");
	TRAP::Graphics::TextureManager::Load("QOITest32BPPsRGB", "./Assets/Textures/QOI/Test32BPPsRGB.qoi");
	TRAP::Graphics::TextureManager::Load("QOITest32BPPLinear", "./Assets/Textures/QOI/Test32BPPLinear.qoi");

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	TP_TRACE("[ImageLoader] Initializing of all Textures took: ", loadingTimer.ElapsedMilliseconds(), "ms!");

	TRAP::Graphics::RenderCommand::SetClearColor();
}

//-------------------------------------------------------------------------------------------------------------------//

void ImageLoaderTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	TRAP::Graphics::Renderer2D::BeginScene(m_camera);
	if(m_tga)
	{
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest16BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest16BPPRLE"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest24BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest24BPPRLE"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest32BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATest32BPPRLE"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATestGrayscale"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("TGATestPalette"));
	}
	else if(m_png)
	{
		//First Row
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest8BPPGrayscaleBig"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest8BPPGrayscaleBigInterlaced"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest8BPPGrayscaleSmaller"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest8BPPGrayscaleSmallest"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest16BPPGrayscaleBig"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest16BPPGrayscaleBigInterlaced"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest16BPPGrayscaleSmaller"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest16BPPGrayscaleSmallest"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest24BPPBig"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest24BPPBigInterlaced"));

		//Second Row
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest24BPPSmaller"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest24BPPSmallest"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest48BPPBig"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest48BPPBigInterlaced"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest48BPPSmaller"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTest48BPPSmallest"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.25f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTestPaletteBig"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.5f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTestPaletteBigInterlaced"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.75f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTestPaletteSmaller"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={1.0f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
											 TRAP::Graphics::TextureManager::Get2D("PNGTestPaletteSmallest"));
	}
	else if(m_bmp)
	{
		//First Row
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPGrayscale"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPGrayscaleReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPGrayscaleRLE"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPGrayscaleRLEReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPPalette"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPPaletteReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPPaletteRLE"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest8BPPPaletteRLEReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest16BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest16BPPReversed"));

		//Second Row
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest16BPPBitFields"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest24BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest24BPPReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest32BPP"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest32BPPReversed"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.0f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("BMPTest32BPPBitFields"));
	}
	else if(m_pm)
	{
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest8BPPGrayscalePGM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest8BPPGrayscalePAM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest16BPPGrayscalePGM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest16BPPGrayscalePAM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest24BPPPPM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest24BPPPAM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest48BPPPPM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTest48BPPPAM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTestGrayscaleHDRPFM"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("PMTestHDRPFM"));
	}
	else if(m_radiance)
	{
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("RadianceTestHDR"));
	}
	else if(m_qoi)
	{
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.25f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("QOITest24BPPsRGB"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-1.0f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("QOITest24BPPLinear"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.75f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("QOITest32BPPsRGB"));
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={-0.5f, 0.25f, 0.0f}, .Rotation={0.0f, 0.0f, 0.0f}, .Scale={0.25f, 0.25f, 0.25f} },
		                                     TRAP::Graphics::TextureManager::Get2D("QOITest32BPPLinear"));
	}
	TRAP::Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

bool ImageLoaderTests::OnKeyPress(const TRAP::Events::KeyPressEvent& event)
{
	if(event.GetKey() == TRAP::Input::Key::Escape)
	{
		TRAP::Application::Shutdown();
		return true;
	}

	if (event.GetKey() == TRAP::Input::Key::One ||
		event.GetKey() == TRAP::Input::Key::Two ||
		event.GetKey() == TRAP::Input::Key::Three ||
		event.GetKey() == TRAP::Input::Key::Four ||
		event.GetKey() == TRAP::Input::Key::Five ||
		event.GetKey() == TRAP::Input::Key::Six)
	{
		m_tga = false;
		m_png = false;
		m_bmp = false;
		m_pm = false;
		m_radiance = false;
		m_qoi = false;
	}

	if (event.GetKey() == TRAP::Input::Key::One)
		m_tga = true;
	else if (event.GetKey() == TRAP::Input::Key::Two)
		m_png = true;
	else if (event.GetKey() == TRAP::Input::Key::Three)
		m_bmp = true;
	else if (event.GetKey() == TRAP::Input::Key::Four)
		m_pm = true;
	else if (event.GetKey() == TRAP::Input::Key::Five)
		m_radiance = true;
	else if (event.GetKey() == TRAP::Input::Key::Six)
		m_qoi = true;

	return true;
}
