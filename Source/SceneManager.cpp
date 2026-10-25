///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

// AI use acknowledgment: ChatGPT assisted with texture mapping, material configuration, and lighting for Modules Five and Six.

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/



// Module Six: lighting and material configuration for the textured scene.
void SceneManager::DefineObjectMaterials()
{
    m_objectMaterials.clear();
    OBJECT_MATERIAL wood = { glm::vec3(0.85f), glm::vec3(0.10f), 8.0f, "matteWood" };
    OBJECT_MATERIAL ceramic = { glm::vec3(0.95f), glm::vec3(0.65f), 48.0f, "glazedCeramic" };
    OBJECT_MATERIAL trim = { glm::vec3(0.95f), glm::vec3(0.80f), 72.0f, "glazedTrim" };
    OBJECT_MATERIAL coffee = { glm::vec3(0.85f), glm::vec3(0.75f), 64.0f, "coffeeLiquid" };
    m_objectMaterials.push_back(wood);
    m_objectMaterials.push_back(ceramic);
    m_objectMaterials.push_back(trim);
    m_objectMaterials.push_back(coffee);
    m_objectMaterials.push_back({ glm::vec3(0.9f), glm::vec3(0.12f), 12.0f, "bookCover" });
    m_objectMaterials.push_back({ glm::vec3(0.92f), glm::vec3(0.55f), 40.0f, "plateGlaze" });
    m_objectMaterials.push_back({ glm::vec3(0.9f), glm::vec3(0.75f), 64.0f, "ornamentFinish" });
}

void SceneManager::SetupSceneLights()
{
    if (m_pShaderManager == nullptr) return;
    m_pShaderManager->setBoolValue(g_UseLightingName, true);
    // Disable unused light types explicitly, avoiding stale shader uniforms.
    m_pShaderManager->setBoolValue("directionalLight.bActive", false);
    m_pShaderManager->setBoolValue("spotLight.bActive", false);
    for (int i = 0; i < 5; ++i)
        m_pShaderManager->setBoolValue("pointLights[" + std::to_string(i) + "].bActive", false);

    // Warm white key light above and to the left of the mug.
    m_pShaderManager->setVec3Value("pointLights[0].position", glm::vec3(-4.0f, 7.0f, 5.0f));
    m_pShaderManager->setVec3Value("pointLights[0].ambient", glm::vec3(0.19f, 0.17f, 0.15f));
    m_pShaderManager->setVec3Value("pointLights[0].diffuse", glm::vec3(0.80f, 0.73f, 0.65f));
    m_pShaderManager->setVec3Value("pointLights[0].specular", glm::vec3(0.80f, 0.75f, 0.70f));
    m_pShaderManager->setBoolValue("pointLights[0].bActive", true);

    // Softer cool fill light reveals the far side without washing out texture.
    m_pShaderManager->setVec3Value("pointLights[1].position", glm::vec3(5.0f, 4.5f, -3.0f));
    m_pShaderManager->setVec3Value("pointLights[1].ambient", glm::vec3(0.04f, 0.05f, 0.07f));
    m_pShaderManager->setVec3Value("pointLights[1].diffuse", glm::vec3(0.23f, 0.30f, 0.40f));
    m_pShaderManager->setVec3Value("pointLights[1].specular", glm::vec3(0.20f, 0.26f, 0.35f));
    m_pShaderManager->setBoolValue("pointLights[1].bActive", true);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// Module Five textures are stored locally with the project so Visual Studio
	// can resolve them reliably when the debugger uses the project directory.
	CreateGLTexture("textures/wood.png", "wood");
	CreateGLTexture("textures/ceramic_body.png", "ceramicBody");
	CreateGLTexture("textures/ceramic_trim.png", "ceramicTrim");
	CreateGLTexture("textures/coffee.png", "coffee");
	CreateGLTexture("textures/book_cover.png", "book");
	CreateGLTexture("textures/plate_glaze.png", "plate");
	CreateGLTexture("textures/ornament.png", "ornament");
	BindGLTextures();

	DefineObjectMaterials();
	SetupSceneLights();

	// Load each primitive once. The coffee mug is assembled from several
	// transformed primitives so that the object remains simple and modular.
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadSphereMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  Renders the Module Seven final project scene: a coffee mug
 *  recreated from multiple primitive meshes. Scaling,
 *  rotation, and translation are used to assemble the parts.
 ***********************************************************/
void SceneManager::RenderScene()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// -----------------------------------------------------------------
	// DESKTOP / GROUND PLANE
	// Provides a simple surface beneath the modeled object.
	// -----------------------------------------------------------------
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	// Tile the wood texture across the desk to demonstrate a complex
	// texturing technique while avoiding visible stretching.
	SetShaderMaterial("matteWood");
	SetShaderTexture("wood");
	SetTextureUVScale(5.0f, 3.0f);
	m_basicMeshes->DrawPlaneMesh();

	// -----------------------------------------------------------------
	// MUG BODY - tapered cylinder
	// The slight taper approximates the wider rim shown on the desk mug.
	// -----------------------------------------------------------------
	scaleXYZ = glm::vec3(2.15f, 3.25f, 2.15f);
	positionXYZ = glm::vec3(0.0f, 1.63f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("glazedCeramic");
	SetShaderTexture("ceramicBody");
	SetTextureUVScale(2.0f, 1.0f);
	m_basicMeshes->DrawTaperedCylinderMesh();

	// -----------------------------------------------------------------
	// TOP RIM - torus
	// A thin torus gives the mug a distinct rounded lip.
	// -----------------------------------------------------------------
	scaleXYZ = glm::vec3(2.18f, 0.20f, 2.18f);
	positionXYZ = glm::vec3(0.0f, 3.25f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("glazedTrim");
	SetShaderTexture("ceramicTrim");
	SetTextureUVScale(2.0f, 1.0f);
	m_basicMeshes->DrawTorusMesh();

	// -----------------------------------------------------------------
	// HANDLE - torus rotated into a vertical orientation
	// Rotation and translation place the handle on the mug's right side.
	// -----------------------------------------------------------------
	scaleXYZ = glm::vec3(1.55f, 1.55f, 0.42f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(2.05f, 1.85f, 0.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees,
		ZrotationDegrees, positionXYZ);
	SetShaderMaterial("glazedCeramic");
	SetShaderTexture("ceramicBody");
	SetTextureUVScale(2.0f, 1.0f);
	m_basicMeshes->DrawTorusMesh();

	// -----------------------------------------------------------------
	// INNER COFFEE SURFACE - shallow cylinder cap
	// Adds depth and makes the mug read as a hollow container.
	// -----------------------------------------------------------------
	scaleXYZ = glm::vec3(1.72f, 0.05f, 1.72f);
	positionXYZ = glm::vec3(0.0f, 3.22f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderMaterial("coffeeLiquid");
	SetShaderTexture("coffee");
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawCylinderMesh();

    // Independent object 2: hardcover book, formed from a box primitive.
    SetTransformations(glm::vec3(2.7f, 0.25f, 1.9f), 0.0f, -12.0f, 0.0f,
        glm::vec3(-4.3f, 0.17f, 0.4f));
    SetShaderMaterial("bookCover");
    SetShaderTexture("book");
    SetTextureUVScale(1.0f, 1.0f);
    m_basicMeshes->DrawBoxMesh();

    // Independent object 3: shallow ceramic plate beside the mug.
    SetTransformations(glm::vec3(1.8f, 0.11f, 1.8f), 0.0f, 0.0f, 0.0f,
        glm::vec3(4.5f, 0.11f, 1.2f));
    SetShaderMaterial("plateGlaze");
    SetShaderTexture("plate");
    SetTextureUVScale(1.0f, 1.0f);
    m_basicMeshes->DrawCylinderMesh();

    // Independent object 4: decorative spherical ornament on the tabletop.
    SetTransformations(glm::vec3(0.9f, 0.9f, 0.9f), 0.0f, 0.0f, 0.0f,
        glm::vec3(4.7f, 0.91f, -2.5f));
    SetShaderMaterial("ornamentFinish");
    SetShaderTexture("ornament");
    SetTextureUVScale(1.0f, 1.0f);
    m_basicMeshes->DrawSphereMesh();
}

