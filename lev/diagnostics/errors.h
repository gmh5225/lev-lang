#pragma once
#include <variant>
#include <format>
#include <source_location>

#include <lev/sourceLocation.h>
#include <lev/parsing/token.h>

namespace lev {

class LexError {
  public:
    struct UnexpectedCharacter {
      char character;
      SourceLocation location;

      UnexpectedCharacter(char character, SourceLocation location)
        : character(character)
        , location(location) {}
    };

    struct RedundantDecimalPoint {
      SourceLocation location;
      RedundantDecimalPoint(SourceLocation location)
        : location(location) {}
    };

    struct UnterminatedString {
      SourceLocation location;
      UnterminatedString(SourceLocation location)
        : location(location) {}
    };

  private:
    using Error = std::variant<UnexpectedCharacter, RedundantDecimalPoint, UnterminatedString>;
    Error mError;

  public:
    template <class T>
    requires std::is_constructible_v<Error, T>
    LexError(T error) : mError(error) {}

    constexpr auto location() const -> SourceLocation {
      return std::visit([](const auto& e) { return e.location; }, mError);
    }

    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, mError);
    }

    auto message() const -> std::string;
};

class ParseError {
  public:
    struct UnexpectedToken {
      TokenType got;
      TokenType expected;
      std::string_view message;
      SourceLocation location;

      UnexpectedToken(TokenType got, TokenType expected, SourceLocation location)
        : got(got)
        , expected(expected)
        , location(location) {}

      UnexpectedToken(TokenType got, TokenType expected, std::string_view message, SourceLocation location)
        : got(got)
        , expected(got)
        , message(message)
        , location(location) {}

      auto format() const -> std::string {
        return std::format("Unexpected token");
      }
    };

    struct Unimplemented {
      std::source_location sourceLocation;
      SourceLocation location;

      Unimplemented(const std::source_location location_ = std::source_location::current())
          : sourceLocation(location_) {}

      auto format() const -> std::string {
        return std::format("Unimplemented Error at: {}{} {}", sourceLocation.line(), sourceLocation.file_name(), sourceLocation.column());
      }
    };

  private:
    using Error = std::variant<UnexpectedToken, Unimplemented>;
    Error mError;

  public:
    template <class T>
    requires std::is_constructible_v<Error, T>
    ParseError(T error) : mError(error) {}

    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, mError);
    }

    auto message() const -> std::string {
      return std::visit([](const auto& e){ return e.format(); }, mError);
    }

    constexpr auto location() const -> SourceLocation {
      return std::visit([](const auto& e) { return e.location; }, mError);
    }

};


struct LevError {
  using Error = std::variant<LexError, ParseError>;

  Error error;
  std::string_view line;

  template <class T>
  requires std::is_constructible_v<Error, T>
  LevError(T error, std::string_view line) 
    : error(error)
    , line(line) {}

  constexpr auto location() -> SourceLocation {
    return std::visit([](const auto& error){ return error.location(); }, error);
  }
  constexpr auto message() -> std::string {
    return std::visit([](const auto& error){ return error.message(); }, error);
  }
};

}
