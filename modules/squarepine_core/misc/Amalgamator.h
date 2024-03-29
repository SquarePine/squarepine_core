#if 0

/* Handles the @remap directive for redirecting includes to amalgamated files.
*/
class RemapTable final : public std::unordered_map<std::string, std::string>
{
public:
    RemapTable() :
        pattern ("[ \t]*/\\*[ \t]*@remap[ \t]+\" ([^\"]+)\"[ \t]+\" ([^\"]+)\"[ \t]*\\*/[ \t]*")
    {
    }

    /** @returns true if the line was an '@remap' directive. */
    bool processLine (const String& line)
    {
        return processLine (line.toStdString());
    }

private:
    std::regex pattern;
    std::unordered_map<std::string, std::string> value;

    bool processLine (const std::string& line)
    {
        std::match_results<std::string::const_iterator> result;

        if (std::regex_match (line, result, pattern))
        {
            if (find (result[1]) == cend())
            {
                this->operator[] (result[1]) = result[2];
                std::cout << "@remap " << result[1] << " to " << result[2] << std::endl;
            }
            else
            {
                std::cout << "Warning: duplicate @remap directive" << std::endl;
            }

            return true;
        }

        return false;
    }
};

//==============================================================================
class IncludeProcessor
{
public:
    IncludeProcessor() :
        includePattern ("[ \t]*#include[ \t]+ (.*)[ t]*"),
        macroPattern (" ([_a-zA-Z][_0-9a-zA-Z]*)"),
        anglePattern ("< ([^>]+)>.*"),
        quotePattern ("\" ([^\"]+)\".*")
    {
    }

    bool processLine (const String& line)
    {
        return processLine (line.toStdString());
    }

private:
    std::regex includePattern,  macroPattern, anglePattern, quotePattern;

    bool processLine (const std::string& line)
    {
        std::match_results<std::string::const_iterator> r1;

        if (std::regex_match (line, r1, includePattern))
        {
            const auto s = std::string (r1[1]);
            std::match_results<std::string::const_iterator> r2;

            if (std::regex_match (s, r2, macroPattern))
            {
                // r2[1] holds the macro
                return true;
            }
            else if (std::regex_match (s, r2, anglePattern))
            {
                // std::cout << "\"" << r2[1] << "\" ";
                return true;
            }
            else if (std::regex_match (s, r2, quotePattern))
            {
                // std::cout << "\"" << r2[1] << "\" ";
                return true;
            }
        }

        return false;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IncludeProcessor)
};

//==============================================================================
class Amalgamator final
{
public:
    explicit Amalgamator (const String& toolName) :
        name (toolName)
    {
        setWildcards ("*.cpp;*.c;*.h;*.hpp;*.mm;*.m");
    }

    const String& getName() const noexcept { return name; }

    void setCheckSystemIncludes (bool shouldCheckSystemIncludes)
    {
        checkSystemIncludes = shouldCheckSystemIncludes;
    }

    void setWildcards (const String& newWildcards)
    {
        wildcards.clear();
        wildcards.addTokens (newWildcards, ";,", "'\"");
        wildcards.trim();
        wildcards.removeEmptyStrings();
    }

    void setTemplate (const String& fileName)
    {
        templateFile = File::getCurrentWorkingDirectory().getChildFile (fileName);
    }

    void setTarget (const String& fileName)
    {
        targetFile = File::getCurrentWorkingDirectory().getChildFile (fileName);
    }

    void setVerbose()
    {
        verbose = true;
    }

    void addDirectoryToSearch (const String& directoryToSearch)
    {
        directoriesToSearch.add (File (directoryToSearch).getFullPathName());
    }

    void addPreventReinclude (const String& identifier)
    {
        preventReincludes.add (identifier);
    }

    void addForceReinclude (const String& identifier)
    {
        forceReincludes.add (identifier);
    }

    void addDefine (const String& defineName, const String& defineValue)
    {
        macrosDefined.set (defineName, defineValue);
    }

    bool process()
    {
        bool error = false;

        if (! templateFile.existsAsFile())
        {
            std::cout << getName() << " The template file doesn't exist!\n\n";
            error = true;
        }

        if (! error)
        {
            // Prepare to write output to a temporary file.
            std::cout << "  Building: " << targetFile.getFullPathName() << "...\n";

            TemporaryFile temp (targetFile);
            auto out = temp.getFile().createOutputStream (1024 * 128);

            if (out == nullptr)
            {
                std::cout << "  \n!! ERROR - couldn't write to the target file: " << temp.getFile().getFullPathName() << "\n\n";
                return false;
            }

            if (! parseFile (targetFile.getParentDirectory(), targetFile,
                             *out, templateFile, alreadyIncludedFiles,
                             includesToIgnore, wildcards, 0, false))
            {
                return false;
            }

            if (calculateFileHashCode (targetFile) == calculateFileHashCode (temp.getFile()))
            {
                std::cout << "   -- No need to write - new file is identical\n";
                return true;
            }

            if (! temp.overwriteTargetFileWithTemporary())
            {
                std::cout << "  \n!! ERROR - couldn't write to the target file: " << targetFile.getFullPathName() << "\n\n";
                return false;
            }
        }

        return error;
    }

    static int64 calculateStreamHashCode (InputStream& in)
    {
        int64 t = 0;

        constexpr auto bufferSize = 4096;
        HeapBlock<uint8> buffer;
        buffer.malloc (bufferSize);

        for (;;)
        {
            const auto num = in.read (buffer, bufferSize);

            if (num <= 0)
                break;

            for (int i = 0; i < num; ++i)
                t = t * 65599 + buffer[i];
        }

        return t;
    }

    static int64 calculateFileHashCode (const File& file)
    {
        auto stream = file.createInputStream();
        return stream != nullptr ? calculateStreamHashCode (*stream) : 0;
    }

private:
    struct ParsedInclude final
    {
        ParsedInclude() = default;

        bool isIncludeLine = false;
        bool preventReinclude = false;
        bool forceReinclude = false;
        int endOfInclude = 0;
        String lineUpToEndOfInclude, lineAfterInclude, filename;
    };

    const String name;
    bool verbose = false, checkSystemIncludes = false;
    StringPairArray macrosDefined;
    StringArray wildcards, forceReincludes, preventReincludes,
                directoriesToSearch, alreadyIncludedFiles,
                includesToIgnore;
    File templateFile, targetFile;
    RemapTable remapTable;
    IncludeProcessor includeProcessor;

    bool matchesWildcard (const String& filename) const noexcept
    {
        for (int i = wildcards.size(); --i >= 0;)
            if (filename.matchesWildcard (wildcards[i], true))
            return true;

        return false;
    }

    static bool canFileBeReincluded (const File& f)
    {
        auto content = f.loadFileAsString();

        for (;;)
        {
            content = content.trimStart();

            if (content.startsWith ("//"))
                content = content.fromFirstOccurrenceOf ("\n", false, false);
            else if (content.startsWith ("/*"))
                content = content.fromFirstOccurrenceOf ("*/", false, false);
            else
                break;
        }

        StringArray lines;
        lines.addLines (content);
        lines.trim();
        lines.removeEmptyStrings();

        const auto l1 = lines[0].removeCharacters (" \t").trim();
        const auto l2 = lines[1].removeCharacters (" \t").trim();
        return l1.replace ("#ifndef", "#define") != l2;
    }

    File findInclude (const File& siblingFile, String filename)
    {
        if (siblingFile.getSiblingFile (filename).existsAsFile())
            return siblingFile.getSiblingFile (filename);

        for (int i = 0; i < directoriesToSearch.size(); ++i)
            if (File (directoriesToSearch[i]).getChildFile (filename).existsAsFile())
                return File (directoriesToSearch[i]).getChildFile (filename);

        return {};
    }

    void findAllFilesIncludedIn (const File& hppTemplate, StringArray& alreadyIncludedFiles)
    {
        StringArray lines;
        lines.addLines (hppTemplate.loadFileAsString());

        for (int i = 0; i < lines.size(); ++i)
        {
            auto line = lines[i];

            if (line.removeCharacters (" \t").startsWithIgnoreCase ("#include\""))
            {
                const auto filename = line.fromFirstOccurrenceOf ("\"", false, false)
                                          .upToLastOccurrenceOf ("\"", false, false);

                const auto targetIncludeFile = findInclude (hppTemplate, filename);

                if (! alreadyIncludedFiles.contains (targetIncludeFile.getFullPathName()))
                {
                    alreadyIncludedFiles.add (targetIncludeFile.getFullPathName());

                    findAllFilesIncludedIn (targetIncludeFile, alreadyIncludedFiles);
                }
            }
        }
    }

    ParsedInclude parseInclude (const String& line, const String& trimmed)
    {
        ParsedInclude parsed;

        if (trimmed.startsWithChar ('#'))
        {
            const auto removed = trimmed.removeCharacters (" \t");

            if (removed.startsWithIgnoreCase ("#include\""))
            {
                parsed.endOfInclude = line.indexOfChar (line.indexOfChar ('\"') + 1, '\"') + 1;
                parsed.filename = line.fromFirstOccurrenceOf ("\"", false, false)
                                       .upToLastOccurrenceOf ("\"", false, false);

                parsed.isIncludeLine = true;
            }
            else if (removed.startsWithIgnoreCase ("#include<") && checkSystemIncludes)
            {
                parsed.endOfInclude = line.indexOfChar (line.indexOfChar ('<') + 1, '>') + 1;
                parsed.filename = line.fromFirstOccurrenceOf ("<", false, false)
                                      .upToLastOccurrenceOf (">", false, false);
                parsed.isIncludeLine = true;
            }
            else if (removed.startsWithIgnoreCase ("#include"))
            {
                String includeName;

                if (line.contains ("/*"))
                    includeName = line.fromFirstOccurrenceOf ("#include", false, false)
                                      .upToFirstOccurrenceOf ("/*", false, false).trim();
                else
                    includeName = line.fromFirstOccurrenceOf ("#include", false, false).trim();

                parsed.endOfInclude = line.upToFirstOccurrenceOf (includeName, true, false).length();

                const auto value = macrosDefined[includeName];

                if (verbose)
                    std::cout << "name = " << name << "\n";

                if (value.startsWithIgnoreCase ("\""))
                {
                    parsed.endOfInclude = line.trimEnd().length() + 1;
                    parsed.filename = value.fromFirstOccurrenceOf ("\"", false, false)
                                           .upToLastOccurrenceOf ("\"", false, false);

                    parsed.isIncludeLine = true;
                }
                else if (value.startsWithIgnoreCase ("<") && checkSystemIncludes)
                {
                    parsed.endOfInclude = line.trimEnd().length() + 1;
                    parsed.filename = value.fromFirstOccurrenceOf ("<", false, false)
                                           .upToLastOccurrenceOf (">", false, false);
                    parsed.isIncludeLine = true;
                }

                parsed.preventReinclude = parsed.isIncludeLine && preventReincludes.contains (includeName);
                parsed.forceReinclude = parsed.isIncludeLine && forceReincludes.contains (includeName);
            }
        }

        return parsed;
    }

    bool parseFile (const File& rootFolder,
                    const File& newTargetFile,
                    OutputStream& dest,
                    const File& file,
                    StringArray& alreadyIncludedFiles,
                    const StringArray& includesToIgnore,
                    const StringArray& wildcards,
                    int level,
                    bool stripCommentBlocks)
    {
        if (! file.exists())
        {
            std::cout << "  !! ERROR - file doesn't exist!";
            return false;
        }

        // Load the entire file as an array of individual lines.
        StringArray lines;
        lines.addLines (file.loadFileAsString());

        // Make sure there is something in the file.

        if (lines.size() == 0)
        {
            std::cout << "  !! ERROR - input file was empty: " << file.getFullPathName();
            return false;
        }

        // Produce some descriptive progress.
        if (verbose)
        {
            if (level == 0)
                std::cout << "  Processing \"" << file.getFileName() << "\"\n";
            else
                std::cout << "  Inlining " << String::repeatedString (" ", level - 1) << "\"" << file.getFileName() << "\"\n";
        }

        bool lastLineWasBlank = true;

        for (int i = 0; i < lines.size(); ++i)
        {
            auto line = lines[i];
            auto trimmed = line.trimStart();

            if (level != 0 && trimmed.startsWith ("//================================================================"))
                line.clear();

            if (remapTable.processLine (line))
            {
                line.clear();
            }
            else
            {
                const auto parsedInclude = parseInclude (line, trimmed);

                if (parsedInclude.isIncludeLine)
                {
                    const auto targetFile = findInclude (file, parsedInclude.filename);

                    if (targetFile.exists())
                    {
                        if (matchesWildcard (parsedInclude.filename.replaceCharacter ('\\', '/'))
                            && ! includesToIgnore.contains (targetFile.getFileName()))
                        {
                            auto fileNamePart = File (parsedInclude.filename).getFileName();
                            auto remapTo = remapTable.find (std::string (fileNamePart.toUTF8()));
                            if (level != 0 && remapTo != remapTable.end())
                            {
                                line = "#include \"";
                                line << String (remapTo->second.c_str()) << "\"" << newLine;

                                findAllFilesIncludedIn (targetFile, alreadyIncludedFiles);
                            }
                            else if (line.containsIgnoreCase ("FORCE_AMALGAMATOR_INCLUDE")
                                     || ! alreadyIncludedFiles.contains (targetFile.getFullPathName()))
                            {
                                if (parsedInclude.preventReinclude)
                                {
                                    alreadyIncludedFiles.add (targetFile.getFullPathName());
                                }
                                else if (parsedInclude.forceReinclude)
                                {
                                }
                                else if (! canFileBeReincluded (targetFile))
                                {
                                    alreadyIncludedFiles.add (targetFile.getFullPathName());
                                }

                                dest << newLine << "/*** Start of inlined file: " << targetFile.getFileName() << " ***/" << newLine;

                                if (! parseFile (rootFolder, newTargetFile,
                                                 dest, targetFile, alreadyIncludedFiles, includesToIgnore,
                                                 wildcards, level + 1, stripCommentBlocks))
                                {
                                    return false;
                                }

                                dest << "/*** End of inlined file: " << targetFile.getFileName() << " ***/" << newLine << newLine;

                                line = parsedInclude.lineAfterInclude;
                            }
                            else
                            {
                                line.clear();
                            }
                        }
                        else
                        {
                            line = parsedInclude.lineUpToEndOfInclude.upToFirstOccurrenceOf ("\"", true, false)
                                    + targetFile.getRelativePathFrom (newTargetFile.getParentDirectory())
                                                .replaceCharacter ('\\', '/')
                                    + "\""
                                    + parsedInclude.lineAfterInclude;
                        }
                    }
                }
            }

            if ((stripCommentBlocks || i == 0) && trimmed.startsWith ("/*") && (i > 10 || level != 0))
            {
                auto originalI = i;
                auto originalLine = line;

                for (;;)
                {
                    auto end = line.indexOf ("*/");

                    if (end >= 0)
                    {
                        line = line.substring (end + 2);

                        // If our comment appeared just before an assertion, leave it in, as it might be useful..
                        if (lines[i + 1].contains ("assert") || lines[i + 2].contains ("assert"))
                        {
                            i = originalI;
                            line = originalLine;
                        }

                        break;
                    }

                    line = lines[++i];

                    if (i >= lines.size())
                        break;
                }

                line = line.trimEnd();
                if (line.isEmpty())
                    continue;
            }

            line = line.trimEnd();

            if (line.isNotEmpty() || ! lastLineWasBlank)
                dest << line << newLine;

            lastLineWasBlank = line.isEmpty();
        }

        return true;
    }
};

#if 0

//==============================================================================
int main (int argc, char* argv[])
{
  bool error = false;
  bool usage = true;

  const String name (argv[0]);

  Amalgamator amalgamator (name);
  bool gotCheckSystem = false;
  bool gotWildcards = false;
  bool gotTemplate = false;
  bool gotTarget = false;

  for (int i = 1; i < argc; ++i)
  {
    String option (argv[i]);

    String value;
    if (i + 1 < argc)
      value = String (argv[i+1]).unquoted();

    if (option.compareIgnoreCase ("-i") == 0)
    {
      ++i;
      if (i < argc)
      {
        amalgamator.addDirectoryToSearch (value);
      }
      else
      {
        std::cout << name << ": Missing parameter for -i\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-f") == 0)
    {
      ++i;
      if (i < argc)
      {
        amalgamator.addForceReinclude (value);
      }
      else
      {
        std::cout << name << ": Missing parameter for -f\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-p") == 0)
    {
      ++i;
      if (i < argc)
      {
        amalgamator.addPreventReinclude (value);
      }
      else
      {
        std::cout << name << ": Missing parameter for -p\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-d") == 0)
    {
      ++i;
      if (i < argc)
      {
        if (value.contains ("="))
        {
          String name = value.upToFirstOccurrenceOf ("=", false, false);
          String define = value.fromFirstOccurrenceOf ("=", false, false);

          amalgamator.addDefine (name, define);
        }
        else
        {
          std::cout << name << ": Incorrect syntax for -d\n";
        }
      }
      else
      {
        std::cout << name << ": Missing parameter for -d\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-w") == 0)
    {
      ++i;
      if (i < argc)
      {
        if (!gotWildcards)
        {
          amalgamator.setWildcards (value);
        }
        else
        {
          std::cout << name << ": Duplicate option -w\n";
          error = true;
          break;
        }
      }
      else
      {
        std::cout << name << ": Missing parameter for -w\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-s") == 0)
    {
      if (!gotCheckSystem)
      {
        amalgamator.setCheckSystemIncludes (true);
        gotCheckSystem = true;
      }
      else
      {
        std::cout << name << ": Duplicate option -w\n";
        error = true;
        break;
      }
    }
    else if (option.compareIgnoreCase ("-v") == 0)
    {
      amalgamator.setVerbose();
    }
    else if (option.startsWith ("-"))
    {
      std::cout << name << ": Unknown option \"" << option << "\"\n";
      error = true;
      break;
    }
    else
    {
      if (!gotTemplate)
      {
        amalgamator.setTemplate (option);
        gotTemplate = true;
      }
      else if (!gotTarget)
      {
        amalgamator.setTarget (option);
        gotTarget = true;
      }
      else
      {
        std::cout << name << ": Too many arguments\n";
        error = true;
        break;
      }
    }
  }

  if (gotTemplate && gotTarget)
  {
    usage = false;

    //amalgamator.print();
    error = amalgamator.process();
  }
  else
  {
    if (argc > 1)
      std::cout << name << " Too few arguments\n";

    error = true;
  }

  if (error && usage)
  {
    std::cout << "\n";
    std::cout << "  NAME" << "\n";
    std::cout << "  " << "\n";
    std::cout << "   " << name << " - produce an amalgamation of C/C++ source files." << "\n";
    std::cout << "  " << "\n";
    std::cout << "  SYNOPSIS" << "\n";
    std::cout << "  " << "\n";
    std::cout << "   " << name << "[-s]" << "\n";
    std::cout << "    [-w {wildcards}]" << "\n";
    std::cout << "    [-f {file|macro}]..." << "\n";
    std::cout << "    [-p {file|macro}]..." << "\n";
    std::cout << "    [-d {name}={file}]..." << "\n";
    std::cout << "    [-i {dir}]..." << "\n";
    std::cout << "     {inputFile} {outputFile}" << "\n";
    std::cout << "  " << "\n";
    std::cout << "  DESCRIPTION" << "\n";
    std::cout << "  " << "\n";
    std::cout << "   Produces an amalgamation of {inputFile} by replacing #include statements with" << "\n";
    std::cout << "   the contents of the file they refer to. This replacement will only occur if" << "\n";
    std::cout << "   the file was located in the same directory, or one of the additional include" << "\n";
    std::cout << "   paths added with the -i option." << "\n";
    std::cout << "   " << "\n";
    std::cout << "   Files included in angle brackets (system includes) are only inlined if the" << "\n";
    std::cout << "   -s option is specified." << "\n";
    std::cout << "  " << "\n";
    std::cout << "   If an #include line contains a macro instead of a string literal, the list" << "\n";
    std::cout << "   of definitions provided through the -d option is consulted to convert the" << "\n";
    std::cout << "   macro into a string." << "\n";
    std::cout << "  " << "\n";
    std::cout << "   A file will only be inlined once, with subsequent #include lines for the same" << "\n";
    std::cout << "   file silently ignored, unless the -f option is specified for the file." << "\n";
    std::cout << "  " << "\n";
    std::cout << "  OPTIONS" << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -s                Process #include lines containing angle brackets (i.e." << "\n";
    std::cout << "                      system includes). Normally these are not inlined." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -w {wildcards}    Specify a comma separated list of file name patterns to" << "\n";
    std::cout << "                      match when deciding to inline (assuming the file can be" << "\n";
    std::cout << "                      located). The default setting is \"*.cpp;*.c;*.h;*.mm;*.m\"." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -f {file|macro}   Force reinclusion of the specified file or macro on" << "\n";
    std::cout << "                      all appearances in #include lines." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -p {file|macro}   Prevent reinclusion of the specified file or macro on" << "\n";
    std::cout << "                      subsequent appearances in #include lines." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -d {name}={file}  Use {file} for macro {name} if it appears in an #include" << "\n";
    std::cout << "                      line." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -i {dir}          Additionally look in the specified directory for files when" << "\n";
    std::cout << "                      processing #include lines." << "\n";
    std::cout << "  " << "\n";
    std::cout << "    -v                Verbose output mode" << "\n";
    std::cout << "\n";
  }

  return error ? 1 : 0;
}

#endif

#endif // Disabled the whole file...
