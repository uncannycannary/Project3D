#include "Graphics.h"
/****************************************************************************/
/*!
\file Graphics.cpp
\author Muhammad Shafik Bin Mazlinan
\par email: cyboryxmen@yahoo.com
\brief
A class that contains all of our graphics functions and handles our shader and OpenGL initialization
*/
/****************************************************************************/
static const unsigned NUM_OF_LIGHT_PARAMETERS = 11;
/****************************************************************************/
/*!
\brief
Default constructor
*/
/****************************************************************************/
Graphics::Graphics()
	:
meshText(NULL),
currentNumOfLights(0),
m_window(NULL),
meshBegin(NULL),
meshEnd(NULL),
textureBegin(NULL),
textureEnd(NULL),
renderingPlane(NULL)
{
}
/****************************************************************************/
/*!
\brief
Default Destructor
*/
/****************************************************************************/
Graphics::~Graphics()
{
	ClearGFXCard();
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}
/****************************************************************************/
/*!
\brief
Initialization of shaders, lighting, parameters and other OpenGL and graphics code necessary
\param window
		gives the graphics object our current window
*/
/****************************************************************************/
void Graphics::Init(GLFWwindow* window)
{
	m_window = window;
	// Set background color to blue
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);
	//Enable back face culling
	glEnable(GL_CULL_FACE);
	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	m_programID = LoadShaders( "Shader//Texture.vertexshader", "Shader//MultiLight.fragmentshader" );

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	for(int index = 0; index < MAX_LIGHTS; ++index)
	{
		char typeBuffer[126];
		char directionBuffer[126];
		char cutoffBuffer[126];
		char innerBuffer[126];
		char exponentBuffer[126];
		char positionBuffer[126];
		char colorBuffer[126];
		char powerBuffer[126];
		char kCBuffer[126];
		char kLBuffer[126];
		char kQBuffer[126];

		sprintf(typeBuffer, "lights[%d].type", index);
		sprintf(directionBuffer, "lights[%d].spotDirection", index);
		sprintf(cutoffBuffer, "lights[%d].cosCutoff", index);
		sprintf(innerBuffer, "lights[%d].cosInner", index);
		sprintf(exponentBuffer, "lights[%d].exponent", index);
		sprintf(positionBuffer, "lights[%d].position_cameraspace", index);
		sprintf(colorBuffer, "lights[%d].color", index);
		sprintf(powerBuffer, "lights[%d].power", index);
		sprintf(kCBuffer, "lights[%d].kC", index);
		sprintf(kLBuffer, "lights[%d].kL", index);
		sprintf(kQBuffer, "lights[%d].kQ", index);

		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, typeBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, directionBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, cutoffBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, innerBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, exponentBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, positionBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, colorBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POWER] = glGetUniformLocation(m_programID, powerBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KC] = glGetUniformLocation(m_programID, kCBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KL] = glGetUniformLocation(m_programID,kLBuffer);
		m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KQ] = glGetUniformLocation(m_programID, kQBuffer);
	}

	glUseProgram(m_programID);

	for(int index = 0; index < MAX_LIGHTS + 1; ++index)
	{
		lights[index] = NULL;
	}
	currentNumOfLights = 0;
}


unsigned Graphics::GetID(const Texture* tex) const
{
	const GLTexture* texture = dynamic_cast<const GLTexture*>(tex);
	if(texture >= textureBegin && texture < textureEnd)
	{
		return texture - textureBegin + 1;
	}

	return 0;
}

unsigned Graphics::GetID(const Mesh* mesh) const
{
	const GLMesh* glMesh = dynamic_cast<const GLMesh*>(mesh);
	if(glMesh >= meshBegin && glMesh < meshEnd)
	{
		return glMesh - meshBegin + 1;
	}

	return 0;
}

void Graphics::GetWindowSize(int*const width, int*const height)
{
	glfwGetWindowSize(m_window, width, height);
}

/****************************************************************************/
/*!
\brief
initializes our text object to a font
\param filepath
		path to a *.tga that contains our font
*/
/****************************************************************************/
void Graphics::InitText(const DrawOrder* meshText)
{
	this->meshText = meshText;
}
/****************************************************************************/
/*!
\brief
Sets our view according to the camera
\param camera
		the camera that our view will be set to
*/
/****************************************************************************/
void Graphics::SetViewAt(const Camera& camera)
{
	viewStack.LoadIdentity();
	Vector3 position, target, up;
	position = camera.ReturnPosition();
	target = camera.ReturnTarget();
	up = camera.ReturnUp();
	viewStack.LookAt(position.x, position.y, position.z, target.x, target.y, target.z, up.x, up.y, up.z);
}
/****************************************************************************/
/*!
\brief
Sets our perspective
\param FOVy
		the field of view of our perspective
\param aspectRatio
		the aspect ratio of our perspective
\param nearPlane
		the distance of a plane from our perspective that all objects behind it are excluded from the perspective
\param farPlane
		the distance of a plane from our perspective that all objects in front of it are excluded from the perspective
*/
/****************************************************************************/
void Graphics::SetProjectionTo(const float FOVy, const float aspectRatio, const float nearPlane, const float farPlane)
{
	Mtx44 projection;
	projection.SetToPerspective(FOVy, aspectRatio, nearPlane, farPlane);
	projectionStack.LoadMatrix(projection);
}
/****************************************************************************/
/*!
\brief
Add a light to the shader
\param light
		pointer to the light used for our shader
*/
/****************************************************************************/
bool Graphics::AddLight(Light* light)
{
	if(currentNumOfLights < MAX_LIGHTS)
	{
		lights[currentNumOfLights] = light;

		glUniform1i(m_parameters[U_NUMLIGHTS], currentNumOfLights + 1);

		glUniform1i(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_TYPE], lights[currentNumOfLights]->type);
		glUniform3fv(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COLOR], 1, &lights[currentNumOfLights]->color.r);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POWER], lights[currentNumOfLights]->power);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KC], lights[currentNumOfLights]->kC);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KL], lights[currentNumOfLights]->kL);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_KQ], lights[currentNumOfLights]->kQ);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COSCUTOFF], lights[currentNumOfLights]->cosCutoff);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_COSINNER], lights[currentNumOfLights]->cosInner);
		glUniform1f(m_parameters[currentNumOfLights*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_EXPONENT], lights[currentNumOfLights]->exponent);

		++currentNumOfLights;
		return true;
	}
	return false;
}
/****************************************************************************/
/*!
\brief
Updates our light
*/
/****************************************************************************/
void Graphics::UpdateLights()
{
	for(Light** light = lights; *light != NULL; ++light)
	{
		const unsigned index = light - lights;
		if((*light)->type == Light::LIGHT_DIRECTIONAL)
		{
			Vector3 lightDir = (*light)->position;
			Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
			glUniform3fv(m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
		}
		else if((*light)->type == Light::LIGHT_SPOT)
		{
			Vector3 lightPosition_cameraspace = viewStack.Top() * (*light)->position;
			glUniform3fv(m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
			Vector3 spotDirection_cameraspace = viewStack.Top() * (*light)->spotDirection;
			glUniform3fv(m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
		}
		else
		{
			Vector3 lightPosition_cameraspace = viewStack.Top() * (*light)->position;
			glUniform3fv(m_parameters[index*NUM_OF_LIGHT_PARAMETERS + U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		}
	}
}
/****************************************************************************/
/*!
\brief
Renders a mesh
*/
/****************************************************************************/
void Graphics::RenderDraw(const DrawOrder& object, const Mtx44& matrix)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * matrix;
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
	if(object.enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * matrix;
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		Color& kAmbient = object.material->kAmbient;
		Color& kDiffuse = object.material->kDiffuse;
		Color& kSpecular = object.material->kSpecular;

		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], object.material->kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	unsigned ID = GetID(object.material->texture);
	if(ID)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	RenderMesh(object.geometry, ID, GL_TRIANGLES);
}

void Graphics::SendMeshInfo(GLMesh* begin, GLMesh* end)
{
	meshBegin = begin;
	meshEnd = end;
	for(GLMesh* mesh = begin; mesh != end; ++mesh)
	{
		unsigned ID = 0;
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, mesh->GetSize() * sizeof(Vertex) * 3, mesh->GetBegin(), GL_STATIC_DRAW);
	}
}

void Graphics::SendTextureInfo(GLTexture* begin, GLTexture* end)
{
	textureBegin = begin;
	textureEnd = end;
	for(GLTexture* tex = begin; tex != end; ++tex)
	{
		unsigned ID = 0;
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->GetWidth(), tex->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, tex->GetBegin());

		glGenerateMipmap( GL_TEXTURE_2D );
		float maxAnisotropy = 16.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)maxAnisotropy );


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

void Graphics::ClearGFXCard()
{
	unsigned id = 0;
	for(GLMesh* mesh = meshBegin; mesh != meshEnd; ++mesh, ++id)
	{
		glDeleteBuffers(1, &id);
	}

	id = 0;
	for(GLTexture* tex = textureBegin; tex != textureEnd; ++tex, ++id)
	{
		glDeleteTextures(1, &id);
	}
}

void Graphics::BindTexture(const Texture* texture) const
{
	const unsigned textureID = GetID(texture);
	if(textureID)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
}

void Graphics::BindMesh(const Mesh* mesh) const
{
	const unsigned meshID = GetID(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, meshID);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color) + sizeof(Vector3)));
}

void Graphics::EnableLighting(const Mtx44& modelView, const Material* material) const
{
	glUniform1i(m_parameters[U_LIGHTENABLED], 1);
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	const Mtx44 modelView_inverse_transpose = modelView.GetInverse().GetTranspose();

	glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

	//load material
	glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &material->kAmbient.r);
	glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &material->kDiffuse.r);
	glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &material->kSpecular.r);
	glUniform1f(m_parameters[U_MATERIAL_SHININESS], material->kShininess);
}

void Graphics::DisableLighting() const
{
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
}


void Graphics::BindMatrix(const Mtx44& matrix) const
{
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &matrix.a[0]);
}

void Graphics::EnableText(const Color& color) const
{
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);

	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
}

void Graphics::DisableText() const
{
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

const Mesh* Graphics::GetRenderingPlane() const
{
	return renderingPlane;
}

/****************************************************************************/
/*!
\brief
Render text as a mesh in the world
*/
/****************************************************************************/
void Graphics::RenderText(const std::string text, const Color color)
{
	if(!meshText->geometry) //Proper error check
	{
		return;
	}

	glDisable(GL_DEPTH_TEST);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);

	glUniform1i(m_parameters[U_LIGHTENABLED], 0);

	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); // 1.0f is the size of one letter.
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		
		const unsigned numofverts = 6;
		unsigned ID = GetID(meshText->material->texture);
		RenderMesh(meshText->geometry ,(unsigned)text[i] * numofverts, numofverts, ID, GL_TRIANGLES);
	}
	
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}
/****************************************************************************/
/*!
\brief
Renders text on the screen
*/
/****************************************************************************/
void Graphics::RenderTextOnScreen(const std::string text, const Color color, const float size, const float x, const float y, const ORIENTATION orientation)
{
	if(!meshText->geometry) //Proper error check
	{
		return;
	}

	glDisable(GL_DEPTH_TEST);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);

	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	Mtx44 ortho;
	int screenX, screenY;
	glfwGetWindowSize(m_window, &screenX, &screenY);
	ortho.SetToOrtho(0, screenX, 0, screenY, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	switch(orientation)
	{
	case ORIENTATION_TOP:
		modelStack.Translate(x, screenY - y, 0);
		modelStack.Scale(size, size, 1);
		break;
	case ORIENTATION_BOTTOM:
		modelStack.Translate(x, y, 0);
		modelStack.Scale(size, size, 1);
		break;
	case ORIENTATION_CENTRE:
		modelStack.Translate(screenX/2 + x, screenY/2 + y, 0);
		modelStack.Scale(size, size, 1);
		break;
	}
	
	for(unsigned i = 0; i < text.length(); ++i)
	{
		const float spacing = 0.7f;
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * spacing, 0, 0);
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		
		const unsigned numofverts = 6;
		unsigned ID = GetID(meshText->material->texture);
		RenderMesh(meshText->geometry, (unsigned)text[i] * numofverts, numofverts, ID, GL_TRIANGLES);
	}

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}
/****************************************************************************/
/*!
\brief
Renders a mesh onto the screen
*/
/****************************************************************************/
void Graphics::RenderMeshOnScreen(const DrawOrder& object, const Mtx44& matrix, const ORIENTATION orientation)
{
	if(!meshText->geometry) //Proper error check
	{
		return;
	}

	Mtx44 ortho;
	int screenX, screenY;
	glfwGetWindowSize(m_window, &screenX, &screenY);
	ortho.SetToOrtho(0, screenX, 0, screenY, -1000, 1000);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	Mtx44 translate;
	modelStack.PushMatrix();
	Vector3 translation;
	switch(orientation)
	{
	case ORIENTATION_TOP:
		translation = matrix * Vector3();
		translate.SetToTranslation(0, screenY - translation.y * 2, 0);
		break;
	case ORIENTATION_BOTTOM:
		translate.SetToIdentity();
		break;
	case ORIENTATION_CENTRE:
		translate.SetToTranslation(screenX/2, screenY/2, 0);
		break;
	}
	modelStack.LoadMatrix(translate * matrix);


	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	unsigned ID = GetID(object.material->texture);
	if(ID)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	RenderMesh(object.geometry, ID, GL_TRIANGLES);

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}
/****************************************************************************/
/*!
\brief
Renders the mesh
\param textureID
		the id of the texture we will render the mesh with
\param mode
		the drawing mode of the render
*/
/****************************************************************************/
void Graphics::RenderMesh(const Mesh* mesh, const unsigned textureID, const unsigned mode)
{
	if(mesh == NULL)
	{
		throw;
	}
	unsigned ID = GetID(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, ID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color) + sizeof(Vector3)));
	glDrawArrays(mode, 0, mesh->GetSize()*3);

	glBindTexture(GL_TEXTURE_2D, 0);
}
/****************************************************************************/
/*!
\brief
renders a portion of the mesh
\param offset
		the offset to the first vertex in the vertex buffer to be rendered
\param count
		the number of vertices to be rendered after the first
\param textureID
		the id of the texture we will render the mesh with
\param mode
		the drawing mode of the render
*/
/****************************************************************************/
void Graphics::RenderMesh(const Mesh* mesh, const unsigned offset, const unsigned count, const unsigned textureID, const unsigned mode)
{
	unsigned ID = GetID(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, ID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color) + sizeof(Vector3)));
	glDrawArrays(mode, offset, count);

	glBindTexture(GL_TEXTURE_2D, 0);
}
/****************************************************************************/
/*!
\brief
Tells OpenGL to draw itself and it's children
*/
/****************************************************************************/
void Graphics::RenderDraw(const DrawOrder* draw)
{
	for(std::vector<DrawOrder*>::const_iterator child = draw->GetChildrenVector().begin(), end = draw->GetChildrenVector().end(); child != end; child++)
	{
		modelStack.PushMatrix();
		modelStack.MultMatrix(draw->transform.TranslationMatrix() * draw->transform.RotationMatrix() * draw->transform.ScalationMatrix());
		RenderDraw(*child);
	}
	//a small check to see weather the draw order is pointing to a geometry before drawing it.
	if(draw->geometry)
	{
		modelStack.PushMatrix();
		modelStack.MultMatrix(draw->transform.TranslationMatrix() * draw->selfTransform.TranslationMatrix() * draw->transform.RotationMatrix() * draw->selfTransform.RotationMatrix() * draw->transform.ScalationMatrix() * draw->selfTransform.ScalationMatrix());
		RenderDraw(*draw, modelStack.Top());
		modelStack.PopMatrix();
	}
}
void Graphics::BeginDrawing() const
{
	//clear depth and color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0); // 1st attribute buffer :vertices
	glEnableVertexAttribArray(1); // 2nd attribute buffer : colors
	glEnableVertexAttribArray(2); // 3rd attribute : normals
	glEnableVertexAttribArray(3); // 4th attribute : UV coordinates
}
void Graphics::EndDrawing() const
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void Graphics::RenderPixels(Color* buffer, Mesh* plane, unsigned sizeX, unsigned sizeY)
{
	unsigned texture;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_FLOAT, buffer);

	glGenerateMipmap( GL_TEXTURE_2D );
	float maxAnisotropy = 16.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)maxAnisotropy );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Mtx44 MVP, ortho, translate, scale;
	ortho.SetToOrtho(0, sizeX, 0, sizeY, -1, 1);
	translate.SetToTranslation(Vector3(sizeX*0.5, sizeY*0.5, 0));
	scale.SetToScale(sizeX, sizeY, 0);
	MVP = ortho * translate * scale;

	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	unsigned ID = GetID(plane);
	glBindBuffer(GL_ARRAY_BUFFER, ID);

	glBindTexture(GL_TEXTURE_2D, texture);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Color) + sizeof(Vector3)));
	glDrawArrays(GL_TRIANGLES, 0, plane->GetSize());

	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteTextures(1, &texture);
}