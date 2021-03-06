#include "OCamlVisitor.h"


/****************************************************
 * {{{1 Traversal dispatch functions
 */

adt_ptr OCamlVisitor::cached (clang::QualType p, adt_ptr value)
{ return sharing && share_types ? ctx.cache.cached_value (p, value) : nullptr; }

adt_ptr OCamlVisitor::cached (clang::TypeLoc p, adt_ptr value)
{ return sharing && share_type_locs ? ctx.cache.cached_value (p, value) : nullptr; }

adt_ptr OCamlVisitor::cached (clang::DesignatedInitExpr::Designator p, adt_ptr value)
{ return nullptr; }

adt_ptr OCamlVisitor::cached (clang::OffsetOfNode p, adt_ptr value)
{ return nullptr; }

adt_ptr OCamlVisitor::cached (clang::Decl *p, adt_ptr value)
{ return nullptr; }

adt_ptr OCamlVisitor::cached (clang::Stmt *p, adt_ptr value)
{ return nullptr; }

adt_ptr OCamlVisitor::cached (clang::CXXBaseSpecifier p, adt_ptr value)
{ return nullptr; }


void OCamlVisitor::dump (clang::TypeLoc TL)
{ dump (TL.getType ()); }

void OCamlVisitor::dump (clang::DesignatedInitExpr::Designator const &p)
{ puts ("<clang::DesignatedInitExpr::Designator>"); }

void OCamlVisitor::dump (clang::OffsetOfNode const &p)
{ puts ("<clang::OffsetOfNode>"); }

void OCamlVisitor::dump (clang::CXXBaseSpecifier const &p)
{ puts ("<clang::CXXBaseSpecifier>"); }


// Overloads to call appropriate traversal functions.
void OCamlVisitor::traverse (clang::Decl *D)
{ assert (D != nullptr); traverse<clang::Decl *, &OCamlVisitor::TraverseDecl> (D); }

void OCamlVisitor::traverse (clang::Stmt *S)
{ assert (S != nullptr); traverse<clang::Stmt *, &OCamlVisitor::TraverseStmt> (S); }

void OCamlVisitor::traverse (clang::TypeLoc TL)
{ assert (TL); traverse<clang::TypeLoc, &OCamlVisitor::TraverseTypeLoc> (TL); }

void OCamlVisitor::traverse (clang::QualType T)
{ assert (!T.isNull ()); traverse<clang::QualType, &OCamlVisitor::TraverseType> (T); }

void OCamlVisitor::traverse (clang::TypeSourceInfo *TSI)
{ assert (TSI != nullptr); traverse (TSI->getTypeLoc ()); }

// This is an object without a notion of nullability, so the
// assert is missing.
void OCamlVisitor::traverse (clang::DesignatedInitExpr::Designator const &D,
                             clang::DesignatedInitExpr *S)
{ traverse<clang::DesignatedInitExpr::Designator, clang::DesignatedInitExpr *, &OCamlVisitor::TraverseDesignator> (D, S); }

void OCamlVisitor::traverse (clang::OffsetOfNode const &N,
                             clang::OffsetOfExpr *S)
{ traverse<clang::OffsetOfNode, clang::OffsetOfExpr *, &OCamlVisitor::TraverseOffsetOfNode> (N, S); }

void OCamlVisitor::traverse (clang::CXXBaseSpecifier const &B)
{ traverse<clang::CXXBaseSpecifier const &, &OCamlVisitor::TraverseCXXBaseSpecifier> (B); }


// }}}


/****************************************************
 * {{{1 Clang reference management
 */

clang_ref<Decl> OCamlVisitor::ref (clang::Decl *D)
{ return ctx.refs.create<Decl> (D); }

clang_ref<Stmt> OCamlVisitor::ref (clang::Stmt *S)
{ return ctx.refs.create<Stmt> (S); }

clang_ref<Expr> OCamlVisitor::ref (clang::Expr *E)
{ return ctx.refs.create<Expr> (E); }

clang_ref<Ctyp> OCamlVisitor::ref (clang::QualType T)
{ return ctx.refs.create<Ctyp> (T); }

clang_ref<Tloc> OCamlVisitor::ref (clang::TypeLoc TL)
{ return ctx.refs.create<Tloc> (TL); }


// }}}


OCamlVisitor::OCamlVisitor (clang_context &ctx)
  : ctx (ctx)
{
}


void
OCamlVisitor::throw_size_error (std::string input, size_t count)
{
  std::ostringstream message;
  message << "Traversal function postcondition not held:\n\t"
          << "must create exactly 1 value, but created " << count << "\n\t"
          << "input type: " << input << "\n\t"
          << "output types:";
  stack.dump (message, count);
  throw std::runtime_error (message.str ());
}
