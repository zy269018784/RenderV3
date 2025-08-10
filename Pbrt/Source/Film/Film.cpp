#include <Film/Film.h>
namespace Render {
	std::string Film::GetFilename() const {
		auto get = [&](auto ptr) { return ptr->GetFilename(); };
		return DispatchCPU(get);
	}
}
