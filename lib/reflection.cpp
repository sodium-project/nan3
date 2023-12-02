#include "reflection.hpp"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/Sema/ParsedAttr.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "llvm/IR/Attributes.h"

namespace nan3 {

struct ReflectAttrInfo : public clang::ParsedAttrInfo {
  ReflectAttrInfo() {
    OptArgs = 0;
    // GNU-style __attribute__((reflect)) and C++/C2x [[reflect]] and
    // [[nan3::reflect]] supported.
    static constexpr Spelling ReflectSpellings[] = {
        {clang::ParsedAttr::AS_GNU, "reflect"},
        {clang::ParsedAttr::AS_C2x, "reflect"},
        {clang::ParsedAttr::AS_CXX11, "reflect"},
        {clang::ParsedAttr::AS_CXX11, "nan3::reflect"}};
    Spellings = ReflectSpellings;
  }

  bool diagAppertainsToDecl(clang::Sema &Semantic,
                            const clang::ParsedAttr &Attribute,
                            const clang::Decl *Declaration) const override {
    // This attribute appertains to enums or classes only.
    if (isa<clang::EnumDecl>(Declaration) ||
        isa<clang::RecordDecl>(Declaration)) {
      return true;
    }

    Semantic.Diag(Attribute.getLoc(),
                  clang::diag::warn_attribute_wrong_decl_type_str)
        << Attribute << Attribute.isRegularKeywordAttribute()
        << "enums or classes";
    return false;
  }

  AttrHandling
  handleDeclAttribute(clang::Sema &Semantic, clang::Decl *Declaration,
                      const clang::ParsedAttr &Attribute) const override {
    // Check if the declaration is at file scope.
    if (!Declaration->getDeclContext()->isFileContext()) {
      unsigned ID = Semantic.getDiagnostics().getCustomDiagID(
          clang::DiagnosticsEngine::Error,
          "'reflect' attribute only allowed at file scope");
      Semantic.Diag(Attribute.getLoc(), ID);
      return AttributeNotApplied;
    }

    // Check that the attribute has no extra arguments.
    if (Attribute.getNumArgs() != 0) {
      unsigned ID = Semantic.getDiagnostics().getCustomDiagID(
          clang::DiagnosticsEngine::Error,
          "'reflect' attribute requires zero arguments");
      Semantic.Diag(Attribute.getLoc(), ID);
      return AttributeNotApplied;
    }

    // Attach an annotate attribute to the declaration.
    Declaration->addAttr(clang::AnnotateAttr::Create(
        Semantic.Context, "reflect", nullptr, 0, Attribute.getRange()));
    return AttributeApplied;
  }
};

} // namespace nan3

static clang::ParsedAttrInfoRegistry::Add<nan3::ReflectAttrInfo> Attr("reflect",
                                                                      "");