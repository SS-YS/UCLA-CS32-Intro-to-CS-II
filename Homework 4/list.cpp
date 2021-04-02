void listAll(const File* f, string path)  // two-parameter overload
{
    path += "/" + f->name();
    cout << path << endl;

    if (f->files() != nullptr) // it's not a plain file
    {
        const vector<File*>& subFiles = *(f->files());
        for (int i = 0; i < subFiles.size(); i++)
            listAll(subFiles[i], path);
    }
}
