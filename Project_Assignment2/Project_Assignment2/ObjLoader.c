#include "ObjLoader.h"
#include <stdlib.h>
#pragma warning( disable : 6011)
#pragma warning( disable : 6386)
/// <summary>
/// Called by GLUT when the window is closed. Release any previously allocated resources here.
/// </summary>
/// <param name="mesh"></param>
void close(MeshObject* mesh) {
	freeMeshObject(mesh);
}
/// <summary>
/// it cleans the mesh object data.
/// </summary>
/// <param name="object"></param>
void freeMeshObject(MeshObject* object) {
	if (object != NULL) {
		free(object->vertices);
		free(object->texCoords);
		free(object->normals);

		if (object->faces != NULL) {
			for (int i = 0; i < object->faceCount; i++) {
				free(object->faces[i].points);
				object->faces[i].points = NULL;  // Initialize to NULL after freeing
			}

			free(object->faces);
			object->faces = NULL;  // Initialize to NULL after freeing
		}

		free(object);
	}
}
/// <summary>
/// Render the faces of the specified Mesh Object in OpenGL.
/// </summary>
/// <param name="object"></param>
void renderMeshObject(MeshObject* object) {
	for (int faceNo = 0; faceNo < object->faceCount; faceNo++) {
		MeshObjectFace face = object->faces[faceNo];
		if (face.pointCount >= 3) {
			glBegin(GL_POLYGON);

			for (int pointNo = 0; pointNo < face.pointCount; pointNo++) {
				MeshObjectFacePoint point = face.points[pointNo];

				if (point.normalIndex >= 0) {
					Vector3f normal = object->normals[point.normalIndex];
					glNormal3d(normal.x, normal.y, normal.z);
				}

				if (point.texCoordIndex >= 0) {
					Vector2f texCoord = object->texCoords[point.texCoordIndex];
					glTexCoord2d(texCoord.x, texCoord.y);
				}

				Vector3f vertex = object->vertices[point.vertexIndex];
				glVertex3f(vertex.x, vertex.y, vertex.z);
			}

			glEnd();
		}
	}
}
/// <summary>
/// Initialise the specified Mesh Object Face from a string of face data in the Wavefront OBJ file format.
/// </summary>
/// <param name="face"></param>
/// <param name="faceData"></param>
/// <param name="maxFaceDataLength"></param>
void initMeshObjectFace(MeshObjectFace* face, char* faceData, int maxFaceDataLength) {
	int maxPoints = 0;
	int inWhitespace = 0;
	const char* delimiter = " ";
	char* token = NULL;
	char* context = NULL;

	// Do a quick scan of the input string to determine the maximum number of points in this face by counting
	// blocks of whitespace (each point must be preceded by at least one space). Note that we may end up with
	// fewer points than this if one or more prove to be invalid.
	for (int i = 0; i < maxFaceDataLength; i++)
	{
		char c = faceData[i];
		if (c == '\0') {
			break;
		}
		else if ((c == ' ') || (c == '\t')) {
			if (!inWhitespace) {
				inWhitespace = 1;
				maxPoints++;
			}
		}
		else {
			inWhitespace = 0;
		}
	}

	// Parse the input string to extract actual face points (if we're expecting any).
	face->pointCount = 0;
	if (maxPoints > 0) {
		face->points = malloc(sizeof(MeshObjectFacePoint) * maxPoints);

		token = strtok_s(faceData, delimiter, &context);
		while ((token != NULL) && (face->pointCount < maxPoints)) {
			MeshObjectFacePoint parsedPoint = { 0, 0, 0 }; // At this point we're working with 1-based indices from the OBJ file.

			if (strcmp(token, "f") != 0) {
				// Attempt to parse this face point in the format "v/t[/n]" (vertex, texture, and optional normal).
				if (sscanf_s(token, "%d/%d/%d", &parsedPoint.vertexIndex, &parsedPoint.texCoordIndex, &parsedPoint.normalIndex) < 2) {
					// That didn't work out: try parsing in the format "v[//n]" instead (vertex, no texture, and optional normal).
					sscanf_s(token, "%d//%d", &parsedPoint.vertexIndex, &parsedPoint.normalIndex);
				}

				// If we parsed a valid face point (i.e. one that at least contains the index of a vertex), add it.
				if (parsedPoint.vertexIndex > 0) {
					// Adjust all indices down by one: Wavefront OBJ uses 1-based indices, but our arrays are 0-based.
					parsedPoint.vertexIndex--;

					// Discard any negative texture coordinate or normal indices while adjusting them down by one.
					parsedPoint.texCoordIndex = (parsedPoint.texCoordIndex > 0) ? parsedPoint.texCoordIndex - 1 : -1;
					parsedPoint.normalIndex = (parsedPoint.normalIndex > 0) ? parsedPoint.normalIndex - 1 : -1;

					memcpy_s(&face->points[face->pointCount], sizeof(MeshObjectFacePoint), &parsedPoint, sizeof(MeshObjectFacePoint));
					face->pointCount++;
				}
			}

			token = strtok_s(NULL, delimiter, &context);
		}

		// If we have fewer points than expected, free the unused memory.
		if (face->pointCount == 0) {
			free(face->points);
			face->points = NULL;
		}
		else if (face->pointCount < maxPoints) {
			MeshObjectFacePoint* resizedPoints = realloc(face->points, sizeof(MeshObjectFacePoint) * face->pointCount);
			if (resizedPoints != NULL) {
				face->points = resizedPoints;
			}
			else {
				// Handle the case when realloc fails to allocate memory
				// You can choose to free the original face->points and handle the error accordingly
				free(face->points);
				face->points = NULL;
				// Additional error handling code if needed
			}
		}
	}
	else
	{
		face->points = NULL;
	}
}
/// <summary>
/// Loads the meshobject and automatically caclulates the vertices, normals, face, facecounts, the file name must be given.
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
MeshObject* loadMeshObject(const char* fileName) {
	FILE* inFile;
	MeshObject* object = (MeshObject*)malloc(sizeof(MeshObject));
	char line[512];                  // Line currently being parsed
	char keyword[10] = "\0";                // Keyword currently being parsed
	int currentVertexIndex = 0;      // 0-based index of the vertex currently being parsed
	int currentTexCoordIndex = 0;    // 0-based index of the texture coordinate currently being parsed
	int currentNormalIndex = 0;      // 0-based index of the normal currently being parsed
	int currentFaceIndex = 0;        // 0-based index of the face currently being parsed

	if (fopen_s(&inFile, fileName, "r") != 0) {
		return NULL;
	}

	// Allocate and initialize a new Mesh Object.
	if (object == NULL) {
		return NULL;
	}

	object->vertexCount = 0;
	object->vertices = NULL;
	object->texCoordCount = 0;
	object->texCoords = NULL;
	object->normalCount = 0;
	object->normals = NULL;
	object->faceCount = 0;
	object->faces = NULL;
	
	// Pre-parse the file to determine how many vertices, texture coordinates, normals, and faces we have.
	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) {
			if (strcmp(keyword, "v") == 0) {
				object->vertexCount++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				object->texCoordCount++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				object->normalCount++;
			}
			else if (strcmp(keyword, "f") == 0) {
				object->faceCount++;
			}
		}
	}

	if (object->vertexCount > 0)
		object->vertices = (Vector3f*)malloc(sizeof(Vector3f) * object->vertexCount);
	if (object->texCoordCount > 0)
		object->texCoords = (Vector2f*)malloc(sizeof(Vector2f) * object->texCoordCount);
	if (object->normalCount > 0)
		object->normals = (Vector3f*)malloc(sizeof(Vector3f) * object->normalCount);
	if (object->faceCount > 0)
		object->faces = (MeshObjectFace*)malloc(sizeof(MeshObjectFace) * object->faceCount);

	// Parse the file again, reading the actual vertices, texture coordinates, normals, and faces.
	rewind(inFile);
	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) {
			if (strcmp(keyword, "v") == 0) {
				Vector3f vertex = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				if (object != NULL) {
					memcpy_s(&object->vertices[currentVertexIndex], sizeof(Vector3f), &vertex, sizeof(Vector3f));
				}
				currentVertexIndex++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				Vector2f texCoord = { 0, 0 };
				sscanf_s(line, "%*s %f %f", &texCoord.x, &texCoord.y);
				if (object != NULL) {
					memcpy_s(&object->texCoords[currentTexCoordIndex], sizeof(Vector2f), &texCoord, sizeof(Vector2f));
				}
				currentTexCoordIndex++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				Vector3f normal = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
				if (object != NULL) {
					memcpy_s(&object->normals[currentNormalIndex], sizeof(Vector3f), &normal, sizeof(Vector3f));
				}
				currentNormalIndex++;
			}
			else if (strcmp(keyword, "f") == 0) {
				if (object != NULL) {
					initMeshObjectFace(&(object->faces[currentFaceIndex]), line, _countof(line));
				}
				currentFaceIndex++;
			}
		}
	}


	fclose(inFile);

	return object;
}
/// <summary>
/// this method loads the ppm image and stored in GLint texture data.
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
int loadPPM(char* fileName)
{
	FILE* inFile; //File pointer
	int width, height, maxVal; //image metadata from PPM file format
	int totalPixels; // total number of pixels in the image

	// temporary character
	char tempChar;
	// counter variable for the current pixel in the image
	int i;

	char header[100]; //input buffer for reading in the file header information

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	GLubyte* texture; //the texture buffer pointer

	//create one texture with the next available index
	GLuint textureID;
	glGenTextures(1, &textureID);

	if (fopen_s(&inFile, fileName, "r") != 0) {
		return -1;
	}
	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf_s(inFile, "%[^\n] ", header, (unsigned)_countof(header));

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((header[0] != 'P') || (header[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// we have a PPM file
	printf("This is a PPM file\n");

	// read in the first character of the next line
	fscanf_s(inFile, "%c", &tempChar, (unsigned)(&tempChar));

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf_s(inFile, "%[^\n] ", header,(unsigned)_countof(header));

		// print the comment
		printf("%s\n", header);

		// read in the first character of the next line
		fscanf_s(inFile, "%c", &tempChar, (unsigned)(&tempChar));
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, inFile);

	// read in the image hieght, width and the maximum value
	fscanf_s(inFile, "%d %d %d", &width, &height, &maxVal);
	// print out the information about the image file
	printf("%d rows  %d columns  max value= %d\n", height, width, maxVal);

	// compute the total number of pixels in the image
	totalPixels = width * height;

	// allocate enough memory for the image  (3*) because of the RGB data
	
	texture = (GLubyte*)malloc(3 * sizeof(GLuint) * totalPixels);
	if (texture == NULL) {
		return -1;
	}

	// determine the scaling for RGB values
	RGBScaling = 255.0f / maxVal;

	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxVal == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf_s(inFile, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			if (texture != NULL) {
				texture[3 * totalPixels - 3 * i - 3] = red;
				texture[3 * totalPixels - 3 * i - 2] = green;
				texture[3 * totalPixels - 3 * i - 1] = blue;
			}
		}
	}
	else  // need to scale up the data values
	{
	
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf_s(inFile, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			if (texture != NULL) {
				texture[3 * totalPixels - 3 * i - 3] = (GLubyte)(red * RGBScaling);
				texture[3 * totalPixels - 3 * i - 2] = (GLubyte)(green * RGBScaling);
				texture[3 * totalPixels - 3 * i - 1] = (GLubyte)(blue * RGBScaling);
			}
		}
	}


	fclose(inFile);

	glBindTexture(GL_TEXTURE_2D, textureID);


	//Set the texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	//Create mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, (GLuint)width, (GLuint)height, GL_RGB, GL_UNSIGNED_BYTE, texture);

	//openGL guarantees to have the texture data stored so we no longer need it
	free(texture);

	//return the current texture id
	return(textureID);
}