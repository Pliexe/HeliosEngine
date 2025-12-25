#include "ProjectExplorer.h"
#include <Helios/Utils/ShowMessage.h>
#include <cstdio>
#include <cstring>
#include <queue>

#include "Application.h"
#include "Icons.h"
#include "ProjectManager.h"

#include "AssetRegistry.h"
#include "imgui.h"

#ifdef HELIOS_PLATFORM_LINUX
#include <limits.h>
#endif

#define ITEM_SIZE 100.0f
#define ITEM_PADDING 20.0f

#define ALLOC_TRY_SIZE 10

FileType create_item = FileType::None;
int editing_item = -1;
std::filesystem::path currentPath;

class Selection {
private:
	int* arr = nullptr;
	size_t size = 0;
	size_t alloced = ALLOC_TRY_SIZE;

	int highest = 0;
	int lowest = 0;
public:

	Selection(const Selection&) = delete;
	Selection() {
		arr = (int*)malloc(sizeof(int) * alloced);
		if (arr == NULL) {
			//Helios::DepricatedHelios::ShowMessage("Critical!", "Out of memory!", MB_ICONERROR, true);
			exit(-3);
		}
	}

	int GetHighest() {
		return highest;
	}

	int GetLowest() {
		return lowest;
	}

	void SelectNext() {
		if (isEmpty()) return;
		Set(GetHighest() + 1);
	}
	
	void SelectPrevious() {
		if(isEmpty()) return;
		Set(GetLowest() - 1);
	}

	void SelectUp(int rows) {
		if(isEmpty()) return;
		Set(GetHighest() - rows);
	}

	void SelectDown(int rows) {
		if(isEmpty()) return;
		Set(GetHighest() + rows);
	}


	int GetFirst() {
		if(size > 0)
			return *arr;

		return -1;
	}

	bool IsSelected(int x) {
		for(int i = 0; i < size; i++)
			if (*(arr + i) == x) {
				return true;
				break;
			}

		return false;
	}

	void Clear() {
		size = 0;
	}

	void Add(int x) {
		if (size > alloced)
		{
			alloced += ALLOC_TRY_SIZE;
			int* tmp = arr;
			arr = (int*)malloc(sizeof(int) * alloced);
			if (!arr) { 
				std::printf("Out of memory!\n");
				exit(-3);
			}
			memcpy(arr, tmp, size * sizeof(int));
			if (tmp != nullptr) free(tmp);
		}
		*(arr + size) = x;
		if (x > highest) highest = x;
		else if (x < lowest) lowest = x;
		size++;
	}

	void AddInRange(int from, int to) {
		for (int i = from; i < to; i++)
			Add(i);
	}

	void Set(int x) {
		size = 1;
		if (x > highest) highest = x;
		else if (x < lowest) lowest = x;
		*arr = x;
	}

	bool isEmpty() {
		return size == 0;
	}

	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = int;
		using pointer = int*;  // or also value_type*
		using reference = int&;  // or also value_type&
		Iterator(pointer ptr) : m_ptr(ptr) {}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }

		// Prefix increment
		Iterator& operator++() { m_ptr++; return *this; }

		// Postfix increment
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		pointer m_ptr;
	};

	Iterator begin() { return Iterator(&arr[0]); }
	Iterator end() { return Iterator(&arr[size-1]); }

	//int& operator[] (int index);
};

Selection selection;

ImTextureRef GetFileIcon(std::filesystem::path file) {
	if (!file.has_extension()) return Helios::HeliosEditor::ICON_FILE_UNKNOWN->GetTextureID();

	std::string ext = file.extension().generic_string();
	if (ext == ".cpp") return Helios::HeliosEditor::ICON_FILE_CPP->GetTextureID();
	else if (ext == ".hpp") return Helios::HeliosEditor::ICON_FILE_HPP->GetTextureID();
	else if (ext == ".c") return Helios::HeliosEditor::ICON_FILE_C->GetTextureID();
	else if (ext == ".h") return Helios::HeliosEditor::ICON_FILE_H->GetTextureID();
	else if (ext == ".rs") return Helios::HeliosEditor::ICON_FILE_RUST->GetTextureID();
	else if (ext == ".png" || ext == ".jpg") {
		if (Helios::AssetRegistry::GetTextures().find(file.string()) != Helios::AssetRegistry::GetTextures().end())
			return (ImTextureID)Helios::AssetRegistry::GetTextures()[file.string()]->GetTextureID();
		else
			return Helios::HeliosEditor::ICON_FILE_IMAGE->GetTextureID();
	}
	else if (ext == ".txt") return Helios::HeliosEditor::ICON_FILE_TXT->GetTextureID();
	else if (ext == ".scene") return Helios::HeliosEditor::ICON_FILE_SCENE->GetTextureID();
	else if (ext == ".ttf") return Helios::HeliosEditor::ICON_FILE_FONT->GetTextureID();
	else return Helios::HeliosEditor::ICON_FILE_UNKNOWN->GetTextureID();
}

ImTextureRef GetFileIcon(FileType type) {
	switch (type)
	{
	case FileType::Cpp: return Helios::HeliosEditor::ICON_FILE_CPP->GetTextureID();
	case FileType::Hpp: return Helios::HeliosEditor::ICON_FILE_HPP->GetTextureID();
	case FileType::C: return Helios::HeliosEditor::ICON_FILE_C->GetTextureID();
	case FileType::H: return Helios::HeliosEditor::ICON_FILE_H->GetTextureID();
	case FileType::Rust: return Helios::HeliosEditor::ICON_FILE_RUST->GetTextureID();
	case FileType::Folder: return Helios::HeliosEditor::ICON_FOLDER_EMPTY->GetTextureID();
		//case FileType::; return *ICON_FILE_IMAGE;
		//case FileType::; return *ICON_FILE_TXT;
		//case FileType::; return *ICON_FILE_FONT;
	default:
		return Helios::HeliosEditor::ICON_FILE_UNKNOWN->GetTextureID();
	}
}

std::string GetFileExtension(FileType type)
{
	switch (type)
	{
	case FileType::Cpp:
		return ".cpp";
	case FileType::Hpp:
		return ".hpp";
	case FileType::C:
		return ".c";
	case FileType::H:
		return ".h";
	case FileType::Rust:
		return ".rs";
	default:
		return "";
	}
}

void EditItemName(std::filesystem::path entry, std::string new_name) {
	/*if (!MoveFile(entry.wstring().c_str(), (entry.parent_path() / (new_name + entry.extension().string())).wstring().c_str()))
		Helios::DepricatedHelios::ShowMessage("Error", "Unable to rename item", MB_ICONERROR);*/
}

void FileNameOrEdit(std::filesystem::path entry, int i) {
	if (editing_item == i) 
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		static const int MAX_PATH_V = MAX_PATH;
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
		static const int MAX_PATH_V = PATH_MAX;
#endif
		static char name[MAX_PATH_V];
		name[0] = '\0';

		ImGui::SetNextItemWidth(ITEM_SIZE);
		ImGui::SetKeyboardFocusHere();
		if (ImGui::InputText("##new_name", name, MAX_PATH_V - entry.string().length() - 1, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::IsItemDeactivatedAfterEdit()) {
			if (name[0] != '\0')
			{
				if (create_item == FileType::None)
					EditItemName(entry, name);
				else
					CreateFileFromType(entry, create_item, name);	
			}
			create_item = FileType::None;
			editing_item = -1;
		}
	} else {
		std::string name = entry.filename().string();
		ImGui::Button((name.size() > 10) ? (name.substr(0, 8) + std::string("...")).c_str() : name.c_str(), ImVec2(ITEM_SIZE, 20));
	}
}

void ShowFileOrFolder(std::filesystem::path entry, int i, int maxElements) {
	bool is_dir = std::filesystem::is_directory(entry);

	ImGui::SetCursorPos(ImVec2((i % maxElements) * (ITEM_SIZE + ITEM_PADDING) + 20, trunc(i / maxElements) * (ITEM_SIZE + ITEM_PADDING + 20) + 20));
	ImGui::PushID(i);
	ImGui::SetNextItemWidth(200);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, selection.IsSelected(i) ? IM_COL32(100, 100, 100, 255) : IM_COL32(0, 0, 0, 0));
	
	ImGui::PushTabStop(true);
	ImVec2 wndsize = ImVec2(ITEM_SIZE, ITEM_SIZE + 20 + 4);
	ImGui::BeginChild("FolderOrFile", wndsize);
	ImGui::PopTabStop();
	ImGui::PopStyleColor();
	ImGui::PushTabStop(false);

	if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
	{
		if (is_dir)
			currentPath /= entry;
		else
		{
			if(entry.filename().extension() == ".scene")
				Helios::Project::TryLoad(entry);
			else
#ifdef HELIOS_PLATFORM_WINDOWS
				ShellExecute(0, 0, entry.wstring().c_str(), 0, 0, SW_SHOW);
#elif defined(HELIOS_PLATFORM_LINUX)
				system(("xdg-open " + entry.string()).c_str());
#elif defined(HELIOS_PLATFORM_MACOS)
				system(("open " + entry.string()).c_str());
#else
#error "Platform not supported!"
#endif

		}
	} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
			selection.Add(i);
		else if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
		{
			auto highest = selection.GetHighest();
			if (i > highest)
			{
				selection.AddInRange(highest + 1, i);
			}
		}
		else {
			selection.Set(i);
		}
	}
	else if (ImGui::IsKeyDown(ImGuiKey_F2) && selection.GetFirst() == i) {
		editing_item = i;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));

	if(Helios::HeliosEditor::ICON_FOLDER_EMPTY == nullptr)
	{
		Helios::ShowMessage("Critical!", "Error loading icons!", Helios::Message::IconError);
		exit(100);
	}

	if(create_item != FileType::None && i == editing_item)
		ImGui::ImageButton("#create_item", GetFileIcon(create_item), ImVec2(ITEM_SIZE, ITEM_SIZE));
	else if(is_dir)
		ImGui::ImageButton("#create_item", std::filesystem::is_empty(entry) ? Helios::HeliosEditor::ICON_FOLDER_EMPTY->GetTextureID() : Helios::HeliosEditor::ICON_FOLDER->GetTextureID(), ImVec2(ITEM_SIZE, ITEM_SIZE));
	else 
		ImGui::ImageButton("#create_item", GetFileIcon(entry), ImVec2(ITEM_SIZE, ITEM_SIZE));

	ImGuiDragDropFlags src_flags = 0;
	src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
	src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
	if (ImGui::BeginDragDropSource(src_flags))
	{
		if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
			ImGui::Text("%s", entry.string().c_str());
		//ImGui::SetDragDropPayload("DND_EXPLORER_TEXTURE2D", &a, sizeof(int*));
		ImGui::SetDragDropPayload("DND_EXPLORER_TEXTURE2D", entry.string().c_str(), sizeof(const char*) * entry.string().length());
		ImGui::EndDragDropSource();
	}
	
	FileNameOrEdit(entry, i);

	ProjectExplorer_RightClickMenu(entry, i, false);

	ImGui::PopTabStop();
	ImGui::PopStyleColor();

	ImGui::EndChild();
	ImGui::PopID();
}

void CreateFileFromType(std::filesystem::path path, FileType type, std::string name) {
	if (!std::filesystem::is_directory(path))
		path.parent_path();

	if (std::filesystem::exists(path / (name + GetFileExtension(type))))
	{
		int i = 1;
		while (std::filesystem::exists(path / (name + " " + std::to_string(i) + GetFileExtension(type)))) {
			i++;
		}
		path /= name + " " + std::to_string(i) + GetFileExtension(type);
	}
	else path /= name + GetFileExtension(type);

	if (type == FileType::Folder) {
		if(!std::filesystem::create_directory(path))
			Helios::ShowMessage("Error", "Unable to create directory!\n" + Helios::conversions::from_u8string(path.u8string()), Helios::Message::IconError);
	}
	else {
		std::ofstream out(path);

		if (out.is_open()) {
			switch (type)
			{
			case FileType::Cpp:
				out << "#include <Helios.h>" << std::endl;
				out << "class " << std::string_view(name.c_str(), GetFileExtension(type).size() - 1).data();
				break;
			default: break;
			}

			out.close();
		}
		else Helios::ShowMessage("Error", "Unable to create file!\n" + Helios::conversions::from_u8string(path.u8string()), Helios::Message::IconError);
	}
}

void ProjectExplorer_RightClickMenu(std::filesystem::path path, int count, bool is_root)
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
		ImGui::OpenPopup("ExplorerContextMenu");
	if (ImGui::BeginPopup("ExplorerContextMenu")) {
		if (!selection.IsSelected(count)) selection.Set(count);
		if (ImGui::BeginMenu("Create")) {
			
			if (ImGui::MenuItem("Folder"))
			{
				create_item = FileType::Folder;
				editing_item = count;
			}

			ImGui::Separator();
			
			if (ImGui::MenuItem("C++ Script"))
			{
				create_item = FileType::Cpp;
				editing_item = count;
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show in Explorer")) {
			std::string params = "/select,\"" + Helios::conversions::from_u8string(path.u8string()) + "\"";
#ifdef HELIOS_PLATFORM_WINDOWS
			ShellExecuteA(NULL, "open", "explorer.exe", params.c_str(), NULL, SW_NORMAL);
#else
			system(("open " + params).c_str());
#endif
		}

		if (ImGui::MenuItem("Rename", "", false, !is_root))
			editing_item = count;

		if (ImGui::MenuItem("Delete", "", false, !is_root)) {
			/*if (std::filesystem::is_directory(path))
			{
				if (!RemoveDirectory(path.wstring().c_str())) {
					Helios::DepricatedHelios::ShowMessage("Error", "Unable to delete directory!", MB_ICONERROR, true);
				}
			} else if (!DeleteFile(path.wstring().c_str())) {
				Helios::DepricatedHelios::ShowMessage("Error", "Unable to delete file!", MB_ICONERROR, true);
			}*/
		}

		ImGui::Text("Okey %d", count);

		ImGui::EndPopup();
	}
}

void SetCurrentPath(std::filesystem::path assetsPath) { currentPath = assetsPath; }

void ProjectExplorerWindow(std::filesystem::path assetsPath)
{
	if (ImGui::Begin("Explorer")) {

		//currentPath = assetsPath;

		/*if (ImGui::BeginMenuBar()) {
			ImGui::Text(currentPath.string().c_str());

			ImGui::EndMenuBar();

		}*/

		if (ImGui::Button((const char*)assetsPath.filename().u8string().c_str())) currentPath = assetsPath;
		std::filesystem::path tmp = assetsPath;
		for (auto& x : currentPath.lexically_relative(assetsPath))
		{
			if (x.generic_string() != ".") {
				tmp /= x;
				ImGui::SameLine();
				ImGui::Text(">");
				ImGui::SameLine();
				if (ImGui::Button(x.string().c_str())) {
					currentPath = tmp;
				}
			}
		}


		ImGui::BeginChild("FileExplorer");

		int i = 0;
		static int itemSize = 100;
		static int itemPadd = 20;
		int maxElements = (ImGui::GetWindowContentRegionMax().x - 20) / (itemSize + itemPadd);
		if (maxElements != 0) {
			//ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0,0,0,0));

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));



			if (std::filesystem::exists(currentPath) && !currentPath.empty()) {
				std::set<std::filesystem::path> folders;
				std::set<std::filesystem::path> files;


				for (const auto& entry : std::filesystem::directory_iterator(currentPath))
					if (entry.is_directory()) folders.insert(entry);
					else files.insert(entry);


				for (const auto& entry : folders)
				{
					ShowFileOrFolder(entry, i, maxElements);
					i++;
				}

				for (const auto& entry : files)
				{
					ShowFileOrFolder(entry, i, maxElements);
					i++;
				}

				if (create_item != FileType::None)
					ShowFileOrFolder(currentPath, i, maxElements);

				/*if (selection.GetLowest() < maxElements && ImGui::IsKeyReleased(ImGuiKey_RightArrow)) {
					selection.SelectNext();
				}
				else if (selection.GetLowest() > 0 && ImGui::IsKeyReleased(ImGuiKey_LeftArrow)) {
					selection.SelectPrevious();
				}
				else if (selection.GetLowest() + (folders.Size() + files.Size()) > 0 && ImGui::IsKeyReleased(ImGuiKey_UpArrow))
					selection.SelectUp(maxElements);
				else if (selection.GetLowest() + (folders.Size() + files.Size()) > 0 && ImGui::IsKeyReleased(ImGuiKey_UpArrow))
					selection.SelectDown(maxElements);*/

			}

			ProjectExplorer_RightClickMenu(currentPath, i, true);
			ImGui::PopStyleVar();
		}
		


		ImGui::EndChild();
	}
	ImGui::End();
}