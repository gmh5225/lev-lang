#pragma once
#include <expected>
#include <vector>

#include <lev/parsing/token.h>
#include <lev/parsing/lexer.h>
#include <lev/diagnostics/reporter.h>
#include <lev/sourceContext.h>

namespace lev {

class Lev {

public:
  Lev(const char** argv);
  auto compile() -> void;

  auto lex() -> std::vector<Token>;
  auto reset() -> void;

private:
  Lexer mLexer;
  Reporter mReporter;
  SourceContext mSourceContext;
  
private:
  auto processCommandLineArgs(const char**) -> void;

};

}

