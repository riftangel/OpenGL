#include "ogToolbox.h"


glm::mat4 fnCreateTransformationMatrix(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {

	glm::mat4 transmat = glm::mat4(1.0f);
	transmat = glm::translate(transmat, translation);
	transmat = glm::rotate(transmat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transmat = glm::rotate(transmat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transmat = glm::rotate(transmat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transmat = glm::scale(transmat, scale);
	return transmat;
}
