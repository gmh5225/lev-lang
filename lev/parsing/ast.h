#pragma once
#include <variant>
#include <memory>
#include <vector>

#include <lev/parsing/token.h>
#include <lev/parsing/type.h>

namespace lev {

struct Expr {

  struct Binary {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    Token op;

    explicit Binary(Expr left, Expr right, Token op);
  };

  struct Unary {
    std::unique_ptr<Expr> right;
    Token op;

    explicit Unary(Expr right, Token op);
  };

  struct Literal {
    Token value;
    explicit Literal(Token value);
  };

  using ValueType = std::variant<Binary, Unary, Literal>;

  public:
    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, value);
    }

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    Expr(T value) : value(value) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Expr& s1, const Expr& s2) -> bool;
};


struct Stmt {

  struct VariableDeclaration {
    Token identifier;
    LevType type;
    Expr value;

    explicit VariableDeclaration(Token identifier, LevType type, Expr value);
  };

  struct FunctionArgument {
    Token identifier;
    LevType type;

    explicit FunctionArgument(Token identifier, LevType type);
  };

  struct FunctionDeclaration {
    Token identifier;
    std::vector<FunctionArgument> arguments;
    LevType returnType;
    std::unique_ptr<Stmt> body;

    explicit FunctionDeclaration(Token identifier, std::vector<FunctionArgument> arguments, LevType type, Stmt body);
  };

  struct Block {
    std::vector<Stmt> statements;
    explicit Block(std::vector<Stmt> statements);
  };

  using ValueType = std::variant<VariableDeclaration, FunctionDeclaration, Block>;

  public:
    auto accept(auto visitor) -> decltype(auto) {
      return std::visit(visitor, value);
    };

    template <typename T>
    requires std::is_constructible_v<ValueType, T>
    Stmt(T value) : value(std::move(value)) {}

  private:
    ValueType value;

    friend constexpr auto operator==(const Stmt& s1, const Stmt& s2) -> bool;
};

};
