#ifndef SHADER_H
#define SHADER_H

#include <glew.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <vector>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

//Range number
#include <limits>
#include <algorithm>

//used for Text
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <map>

//use for UI
#include <functional>

#include "extern/COMMON.hpp"
#include "extern/GLOBAL_VAR.hpp"
#include "math/mat4.hpp"
#include "world/Scene.hpp"

namespace srw{
	//For 3D engine
	class Camera{
	public:
		Camera(float x, float y, float z, float theta, float near, float far);
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);
		void updateMatrices();
		void updateCoordinates();
		void updateCamera();
		void setFovy(float theta);
		void setPosition(vec3f coordinate);
		mat4<float>& getViewAndProjectionMatrix();
		mat4<float> getViewMatrix();
		mat4<float>& getProjectionMatrix();
		vec3f& getPosition();
		vec3f& getTarget();
		float getFovy() const;
		float getYaw() const;
		float getPitch() const;
	// private:
		//Matrices
		mat4<float> projectionMatrix;
		mat4<float> viewMatrix;
		mat4<float> viewAndProjection;
		
		//Camera control angle view
		float yaw;
		float pitch;
		float fovy;
		float sensitivity;
		float zNear;
		float zFar;

		//Camera control offset
		vec3f position;
		vec3f target;
		bool RIGHT;
		bool FOWARD;
		bool LEFT;
		bool BACKFOWARD;
		float speed;

		//Frame time controls
		float lastFrameTime;
		float currentFrameTime;
		float deltaTime;
	};

	//Wrapper to manage inputs for shaders
	class SRW{
	private:
		struct uniformData{
			std::string uniformName;
		};
		//Use to global scope
		struct uniform3fv : public uniformData{
			uniform3fv(std::string name, vec3f* parm){
				value       = parm;
				uniformName = name;
			}
			vec3f* value;
		};
		struct uniformMat4fv : public uniformData{
			uniformMat4fv(std::string name, mat4<float>* parm){
				value       = parm;
				uniformName = name;
			}
			mat4<float>* value;
		};
		//Use to local scope
		struct uniform1b : public uniformData{
			uniform1b(std::string name, bool parm){
				value       = parm;
				uniformName = name;
			}
			bool value;
		};
		struct uniform1i : public uniformData{
			uniform1i(std::string name, uint parm, uint id = 0){
				value       = parm;
				uniformName = name;
				texID 		= id;
			}
			uint value;
			uint texID;
		};
		struct uniform1f : public uniformData{
			uniform1f(std::string name, float parm){
				value       = parm;
				uniformName = name;
			}
			uniform1f(std::string name, float* parm){
				ptrValue    = parm;
				uniformName = name;
			}
			float value = 0.0f;
			float* ptrValue = NULL;
		};
		struct uniform3f : public uniformData{
			uniform3f(std::string name, vec3f parm){
				value       = parm;
				uniformName = name;
			}
			vec3f value;
		};
	protected:
		void genShader(const char* vs_path, const char* fs_path);
		static bool writeDinamycVar(const char* file_path, uint active_lights);
	public:
		//Constructors
		SRW();
		SRW(const char* vs_path, const char* fs_path, Scene& parm);
		virtual ~SRW();
		
		//To programs
		static uint genProgramID(const char* vs_path, const char* fs_path, Scene* parm = NULL);
		static uint genProgramID(const char* vs_path, const char* fs_path, const char* gs_path);

		//To framebuffers
		static void genCSMFramebuffer(std::vector<uint>& texID, uint& fboID, mat4<float>& matrix, uint width, uint height);
		static void genShadowFramebuffer(uint& texID, uint& fobID, uint width, uint height);
		static void genShadowFramebufferFromCubemap(uint& depthCubemap, uint& fboID, uint width, uint height);
		static void genFramebuffer(uint& texID, uint& rboID, uint& fboID, uint width, uint height);
		static void genGeometryBuffer(uint& gBuffer, uint& gPosition, uint& gNormal, uint& gAlbedo, uint width, uint height);
		
		//Basic setters to init uniforms
		void setCurrentScene(Scene& parm);
		void setUniform1b(std::string, bool value);
		void setUniform1f(std::string name, float* value);
		void setUniform1f(std::string, float value);
		void setUniform3f(std::string, vec3f value);
		void setUniform3fv(std::string, vec3f* value);
		void setUniformMat4fv(std::string name, mat4<float>* value);
		void addTexture(const char* texPath, const char* uniformName, uint texUnit, bool SRGB = false, bool SRGBA = false, uint* parmWidth = NULL, uint* parmHeight = NULL);
		void addTexture(uint& texID, const char* uniformName, uint texUnit, uint uniformType = 0);
		void initUnifSampler2D();
		void initUnifSamplerCubemap();
		void initUnif1b();
		void initUnif1f();
		void initUnif3f();
		void initUnif3fv();
		void initUnifMat4fv();
		void initUnifPointLights();
		void initUnifGlobalLight();
		void initUniforms();
		
		//Define the shading program
		void setProgram(uint& parm);
		uint& getProgram();
		
		//Search for uniform names in uniform arrays
		uint searchUniform1b(std::string name);
		uint searchUniform1f(std::string name);
		uint searchUniform3f(std::string name);
		uint searchUniform3fv(std::string name);
		uint searchUniformMat4fv(std::string name);

		//Update a position in the array of uniforms
		void setUniform1b(uint index, bool value);
		// void setUniform1i(uint index, uint value);
		void setUniform1f(uint index, float value);
		void setUniform3f(uint index, vec3f value);

		mat4<float> getUniformMat4fv(uint index){
			return (*unifMat4fv[index].value);
		}
		vec3f getUniform3f(uint index){
			return unif3f[index].value;
		}
		
		//Displays instance data on console
		void info();
	protected:
		uint program;
		Scene* scene;
		std::vector<uniform1i>     sampler2D;
		std::vector<uniform1i>     samplerCubemap;
		std::vector<uniform1b>	   unif1b;
		std::vector<uniform1f>     unif1f;
		std::vector<uniform3f>     unif3f;
		std::vector<uniform3fv>    unif3fv;
		std::vector<uniformMat4fv> unifMat4fv;
	};

	class CSM{
		public:
			CSM(){
				direction = normalize(vec3f(0.0f, 10.0f, 0.0f));
				direction.show("dir");
			}
			void genFramebuffer(uint width, uint height){
				glGenTextures(maps.size(), maps.data());
				for(uint i = 0; i < maps.size(); ++i){
					glBindTexture(GL_TEXTURE_2D, maps[i]);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
				}
				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, maps[0], 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE){
					std::cout << "Warning: the framebuffer is incomplete!" << std::endl;
					switch(framebufferStatus){
						case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
							std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
						break;
					}
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			void calculateLayerBoundaries(mat4f view_proj){
				mat4f viewToWorld = (view_proj).inverse();
				std::vector<vec4f> frustumCorners;
				for(uint x = 0; x < 2; ++x){
					for(uint y = 0; y < 2; ++y){
						for(uint z = 0; z < 2; ++z){
							vec4f p = vec4f(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f) * viewToWorld;
							frustumCorners.push_back(p / p.w);
						}
					}
				}

				vec3f center;
				for(uint i = 0; i < frustumCorners.size(); ++i){
					center = center + vec3f(frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
				}
				center = center / frustumCorners.size();
				mat4f lightView = lookAtRH(center + direction, vec3f(0.0f, 0.0f, 0.0f));
				float minX = std::numeric_limits<float>::max();
				float maxX = std::numeric_limits<float>::min();
				float minY = std::numeric_limits<float>::max();
				float maxY = std::numeric_limits<float>::min();
				float minZ = std::numeric_limits<float>::max();
				float maxZ = std::numeric_limits<float>::min();
				for(uint i = 0; i < frustumCorners.size(); ++i){
				    vec4f trf = frustumCorners[i] * lightView;
				    minX = std::min(minX, trf.x);
				    maxX = std::max(maxX, trf.x);
				    minY = std::min(minY, trf.y);
				    maxY = std::max(maxY, trf.y);
				    minZ = std::min(minZ, trf.z);
				    maxZ = std::max(maxZ, trf.z);
				}
				std::cout << "maxX" << maxX << std::endl;
				std::cout << "minX" << minX << std::endl;
				std::cout << "minZ" << minZ << std::endl;
				std::cout << "maxZ" << maxZ << std::endl;


				light = lightView * orthographic(maxX, minX, maxY, minY, minZ, maxZ);
			}
		// private:
			vec3f direction;
			uint fbo;
			std::vector<uint> maps;
			std::vector<float> cascades;
			mat4f light;

	};

	//For 3D engine
	class Object : public SRW{
		//New features:
		// - Program shader shadow, minimap id with static members
	private:
		struct vertexAttrib{
			vec3f position;
			vec3f normal;
			vec2f texcoord;
			vec3f tangent;
			void show(){
				position.show("pos");
				normal.show("norm");
				texcoord.show("tex");
			}
		};
	public:	
		//Basic functions 
		Object(const char* model_path, const char* vs_path, const char* fs_path);
		Object(const char* model_path, const char* vs_path, const char* fs_path, Scene& parm);
		Object(const char* model_path, uint& programID);
		Object(const char* model_path, uint& programID, Scene& parm);
		~Object();
		bool parser(const char* model_path);
		void search(std::vector<float>* container, std::ifstream* file, char code);
		void genBuffers();
		void boundingBox();
		void collisionDetection(Object* obj);

		//Rendering
		void render();
		void renderFromDirectionalLightView(uint& programID, mat4<float>& view, mat4<float>& model);
		void renderFromPointLightView(uint& programID, std::vector<mat4<float>>& transforms, mat4<float>& model, vec3f& light_pos, float zFar);
		void renderMinimap(uint& programID, mat4<float>& view, mat4<float>& model);

		//Getters
		std::vector<float> getVertexBufferArray();
		std::string getName();
		uint getVAO() const;
		uint getVBO() const;
		uint getIBO() const;

		//Attributes
		void setDiffuseMap(std::string path);
		void setSpecularMap(std::string path);
		void setDisplacementMap(std::string path);
		void setNormalMap(std::string path);
		void setEmissiveMap(std::string path);

	#if DEBUG
		//Color picking functions
		//Functions
		Object();
		void static createUniformBufferData();
		void static updateUniformBufferData();
		void renderColorPicking();
		static void picking(SDL_Event* event);
		void updateModelViewMatrix();

		//Variables
		static std::vector<Object*> collection;
		static uint programPicking;
	private:
		static uint UBO;
		static uint activeIndex;
		static std::vector<int> offset;
		static bool selected;
		static bool XAXIS;
		static bool YAXIS;
		static bool ZAXIS;
		static int mouseX;
		static int mouseY;
		static int blockSize;
		static uchar data;
		static vec2f currentOffset;
		static vec2f deltaOffset;   
		static vec2f latestOffset;
		static uchar* blockBuffer;
	#endif
	private:
		std::string name;
		uint VAO;
		uint VBO;
		std::vector<float> vertex;
		//AABB
	public:
		vec3f scale_box;
		vec3f pFowardTopRight;
		vec3f pFowardTopLeft;
		vec3f pFowardBotRight;
		vec3f pFowardBotLeft;

		vec3f pBackTopRight;
		vec3f pBackTopLeft;
		vec3f pBackBotRight;
		vec3f pBackBotLeft;
	};

	//For 3D engine
	class Skybox{
		//New features:
		// - Day and Night cycle based at blend cubemaps		
	public:
		//Static members
		static mat4<float> viewAndProjection;
		Skybox(std::string folder, float min = 1.0f);
		~Skybox();

		//Rendering
		void render();		
		void timeline(float ms);
		
		//Setters
		void setOpacity(float alpha);
		void setLightness(float r, float g, float b);
		void setCycleState(bool state);

		//Getters
		bool  getCycleState() const;
		vec3f getLightness();
		float getOpacity() const;
	private:
		float duration;
		float unityOpacity;
		float unityLightness;
		float time;
		bool cycle;
		float opacity;
		vec3f lightness;
		Object cube = Object("models/basic/cube.obj", "../shaders/cubemap/vs_cubemap.glsl", "../shaders/cubemap/fs_cubemap.glsl");
	};

	//Both, 2D and 3D engine
	class Text : public SRW{
	private:
		struct Glyph{
			uint textureID;
			uint width;
	 		uint height;
			int bearingX;
			int bearingY;
			int advance;
		};
	public:
		//Initialize and terminate
		Text(const char* path, uint size);
		void genText(const char* path, uint size);
		void free();

		//Rendering
		void render(std::string text, float x, float y, float scale);

		//Setters
		void setPosition(float x, float y);
		void clipping(bool flag);
		void setBorderMin(float value);
		void setBorderMax(float value);
		void setColor(float r, float g, float b);
	
		//Getters
		vec2f getSize(std::string text);
		vec2f getPosition();
	private:
		uint VAO;
		uint VBO;
		vec2f position;
		vec3f color;
		std::map<char, Glyph> glyphMap;
		//Clipping
		bool clip;
		float borderMin;
		float borderMax;
	};

	enum{
		GRADIENT        = 20,
		UNIFORM         = 21,
		HORIZONTAL      = true,
		VERTICAL        = false,
		BORDERLESS		= -1,
		ALL_BORDER      = 0,
		SIDE_BORDER     = 1,
		VERTICAL_BORDER = 2,
		LEFT_BORDER     = 3,
		RIGHT_BORDER    = 4,
		TOP_BORDER      = 4,
		BOTTOM_BORDER   = 5
	};

	//Both, 2D and 3D engine
	class UI : public SRW{
	public:
		//Constructor and destructor
		UI();
		virtual ~UI();
		//Handle events
		virtual void keyboardEvent(SDL_Event* event) = 0;
		virtual void mouseEvent(SDL_Event* event)    = 0;
		virtual void windowEvent(SDL_Event* event)   = 0;
		
		//Rendering
		virtual void render() = 0;

		//Setters
		void setName(std::string name, float x, float y, const char* fontName, float size);
		void setName(std::string name){
			this->uiName = name;
		}
		void setPosition(float x, float y);
		void setParent(UI* object);
		void setChild(UI* obj);
		void setResolution(float w, float h);
		void setRatio(float x, float y);
		void setScale(float x, float y);
		void hident(bool flag);
		void summon(UI* object);
		void summoner(UI* object);		

		//Getters
		UI* getParent();
		std::vector<UI*>& getChild();
		UI* getSummon();
		UI* getSummoner();
		vec2f getPosition();
		vec2f getResolution();
		vec2f getRatio();
		vec2f getScale();
		Text& getText();
		bool isHidden() const;

		//Generate buffer and update position
		virtual void updateUniforms() = 0;
		void updatePosition();
		void updateActiveRegion(SDL_Event* event);
		void updateBuffer();
		void genBuffer();

		//Shaders parms	
		vec3f on, out, over;
		float borderMin;
		float borderMax;
		bool resetActiveRegion;
		bool enableUI;

		//Auxiliary variables
		static bool lockedUI;
		static mat4<float> uiMatrix;
	protected:
		//Inheritance
		UI* parent;
		std::vector<UI*> child;
		UI* objectSummon;
		UI* objectSummoner;
		
		//Label
		std::string uiName;
		std::string fontFamily;
		float fontSize;
		Text text = Text("../assets/fonts/MAIAN.ttf", 20.0f);	

		//Proportional UI position, resolution and hide variable
		vec2f position;
		vec2f ratio;
		vec2f resolution;
		vec2f currScale;
		// int currWidth;
		// int currHeight;
		bool hided;
		bool inside;

		//OpenGL objects
		uint VAO;
		uint VBO;
	};

	class QueueUI{
	public:
		void put(UI& ui);
		void poll(SDL_Event* event);
		void render();
		void free();
	private:
		std::vector<UI*> queue;
	};

	class Button : public UI{
	public:
		//Initializing
		Button();
		Button(float x, float y, vec2f scale, uint& programID, UI* container = NULL);
		~Button();

		//Handle events
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);

		//Rendering
		void render();
		void updateUniforms();
		
		//UI object function 
		void highlight(std::function<void(void)> func); 		
		void pressed(std::function<void(void)> func);
		void released(std::function<void(void)> func);
		void setAnimationTime(uint time);
		bool isApplied();
		void terminate();
	private:
		//Animation
		bool hold;
		bool applied;
		uint releaseTime;
		uint animationTime;

		//Store previously defined event functions
		std::function<void(void)> highlightFunction;
		std::function<void(void)> pressedFunction;
		std::function<void(void)> releasedFunction;
	};

	class Scroll : public UI{
	public:
		//Construct and destruct
		Scroll();
		~Scroll();

		//Handle events
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);
		
		//Utility functions
		void enable(bool flag);
		bool is();
		void roll(bool flag);
		void save(vec2f coord);
		vec2f saved();
		
		//Rendering
		void render();	
		
		//Update variables
		void updateUniforms();
	private:
		bool scroll;
		bool slide;
		vec2f coordinates;
	};

	class Panel : public UI{
	public:
		//Initializing
		Panel();
		Panel(float x, float y, vec2f scale, uint& programID, UI* container = NULL);
		~Panel();

		//Handle events
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);

		//Rendering
		void render();
		void updateUniforms();
		void scrollbar(bool flag, vec2f scale);
		void resetCoordinates();

		//User-level functions - Interface
		void bordered(vec3f color, float thickness, int mask = UNIFORM, int style = BORDERLESS);
		void fill(vec3f color);
		void opacity(float alpha);
	private:
		Scroll scroll;
	};

	class Quad : public SRW{
	public:
		Quad(){

		}
		Quad(uint choice){
			switch(choice){
				case 0:
					genShader("../shaders/ui/vs_ui.glsl", "../shaders/deferred/fs_render_quad.glsl");
					break;
				case 1:
					genShader("../shaders/ui/vs_ui.glsl", "../shaders/framebuffer/fs_simple_tex.glsl");
					break;
				case 2:
					genShader("../shaders/ui/vs_ui.glsl", "../shaders/framebuffer/fs_shadow_debug.glsl");
					break;
				default:
					break;
			}
			
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
			glBindVertexArray(0);
			setUniformMat4fv("uiMatrix", &UI::uiMatrix);
		}
		void render(float x, float y, float w, float h){
			x = x >= screenWidth ? screenWidth - w: x;
			y = y >= screenHeight ? screenHeight - h: y;
			float vertices[] = {
					 x, y + h, 0.0f, 1.0f,
					 x, y, 0.0f, 0.0f,
					 x + w, y, 1.0f, 0.0f,
					 x + w, y, 1.0f, 0.0f,
					 x + w, y + h, 1.0f, 1.0f,
					 x, y + h, 0.0f, 1.0f
			};
			glUseProgram(program);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			
			initUnifSampler2D();
			initUnif3fv();
			initUnifMat4fv();
			initUnifGlobalLight();
			initUnifPointLights();

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glUseProgram(0);
		}
	private:
		uint VAO;
		uint VBO;
	};

	class SpriteSheet : public SRW{
	public:
		//spitre all type
		SpriteSheet(const char* pathSpriteSheet, float x, float y, float w, float h, uint division, bool flag = false);
		~SpriteSheet();
		void frameTime(uint ms);
		void frameTime(uint frame_id, uint ms);
		void render();
		void windowEvent(SDL_Event* event);
		void addSpriteSheet(const char* pathSpriteSheet, SDL_Scancode key);
		void setRatio(float x, float y);
		void setScale(float w, float h);
		void setPosition(float x, float y);
		void setResolution(float w, float h);	
		vec2f getRatio();
		vec2f getScale();
		vec2f getPosition();
		vec2f getResolution();
		void updatePosition();
		void updateBuffer();

		//generaly, type player
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		bool collision();
			
		//virtual function
		void move();

		//sprite type player and mob
		bool edgeLeft;
		bool edgeRight;
		bool edgeTop;
		bool edgeBottom;

		//sprite all type
		static std::vector<SpriteSheet*> instances;
		bool loop;
		uint end_at;
		uint current_frame;
		uint current_time;
		uint elapsed_time;
		uint frame_time;
		bool hided;
		uint split;
		uint split_size;
		uint iteratorA;
		uint iteratorB;
		std::vector<float> clip;
		std::map<uint, uint> frame;
		uint VAO;
		uint VBO;
		vec2f position;
		vec2f scale;
		vec2f ratio;
		vec2f resolution;
		
		//sprite type player
		bool FOWARD;
		bool BACKFOWARD;
		bool RIGHT;
		bool LEFT;
		std::vector<SDL_Scancode> key;
		float offset_per_frame = 1.0f;
		float velX = 0.0f;
		float velY = 0.0f;
		vec2f velocity;

		//váriaveis locais movidas para arquivo de definição temporariamente
		Quad rect;
		vec2f i_h_box_res;       
		float i_h_b_box_left;  
		float i_h_b_box_right; 
		float i_h_b_box_top;   
		float i_h_b_box_bottom;

		float i_h_t_box_top;
		float i_h_t_box_bottom;

		vec2f i_v_box_res;
		float i_v_r_box_right;
		float i_v_r_box_left;
		float i_v_r_box_top;
		float i_v_r_box_bottom;

		float i_v_l_box_left;
		float i_v_l_box_right;
		float i_v_l_box_top;   
		float i_v_l_box_bottom;

		vec2f p_h_box_res;
		float p_h_b_box_left;
		float p_h_b_box_right;
		float p_h_b_box_top;
		float p_h_b_box_bottom;

		float p_h_t_box_top;
		float p_h_t_box_bottom;

		vec2f p_v_box_res;
		float p_v_l_box_left;
		float p_v_l_box_right;
		float p_v_l_box_top;
		float p_v_l_box_bottom;

		float p_v_r_box_right;
		float p_v_r_box_left;
		float p_v_r_box_top;
		float p_v_r_box_bottom;
	};

	class Sound{
	//[Last upadte 05.09.21]
	//Implementar pilha de dados para processamento paralelo
	public:
		Sound(const char* path = "../assets/bgms/theme.wav");
		static int audioThread(void* ptr);
		void execute(const char* name, int callback(void*));
	};
}	
#endif 

//SRW: liberar explicitamente unidades de texturas após cada loop e outros recursos
