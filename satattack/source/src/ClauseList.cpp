#include "ClauseList.h"
#include "util.h"
#include <map>

namespace AllSAT {
    const int CL_VERBOSE = 8;


    ClauseList::ClauseList()
        : verbose(0)
    {
        lengths.push_back(0);
    }

    ClauseList::~ClauseList()
    {
    }

    void ClauseList::addClause(sat_n::Lit x)
    {
        _ensureLength(_temp_clause, 1);
        _temp_clause[0] = x;
        _addClause(_temp_clause, 1);
    }

    void ClauseList::addClause(sat_n::Lit x, sat_n::Lit y)
    {
        _ensureLength(_temp_clause, 2);
        _temp_clause[0] = x;
        _temp_clause[1] = y;
        _addClause(_temp_clause, 2);
    }

    void ClauseList::addClause(sat_n::Lit x, sat_n::Lit y, sat_n::Lit z)
    {
        _ensureLength(_temp_clause, 3);
        _temp_clause[0] = x;
        _temp_clause[1] = y;
        _temp_clause[2] = z;
        _addClause(_temp_clause, 3);
    }

    void ClauseList::addClause(const sat_n::vec_lit_t& xs)
    {
        _ensureLength(_temp_clause, xs.size());
        for(unsigned i=0; i != xs.size(); i++) {
            _temp_clause[i] = xs[i];
        }
        _addClause(_temp_clause, xs.size());
    }


    void ClauseList::_ensureLength(std::vector<sat_n::Lit>& xs, int size)
    {
        if((int) xs.size() < size) xs.resize(size);
    }

    void ClauseList::_addClause(std::vector<sat_n::Lit>& xs, int size)
    {
        assert(lengths.size() > 0);
        assert(lengths[lengths.size()-1] == (int) clauses.size());
        int clauseIndex = lengths.size()-1;

        for(int i=0; i != size; i++) {
            clauses.push_back(xs[i]);
        }
        lengths.push_back(clauses.size());
        _addWatches(clauseIndex, xs, size);
    }

    void ClauseList::_addWatches(int index, std::vector<sat_n::Lit>& xs, int size)
    {
        using namespace sat_n;
        assert(clauseLen(index) > 0);

        for(int i=0; i != size; i++) {
            int l = toInt(xs[i]);
            assert(l < (int) watches.size() && l >= 0);
            watches[l].push_back(index);
        }
    }

    unsigned ClauseList::clauseLen(int i) const
    {
        assert(i >= 0);
        assert((i+1) < (int) lengths.size());
        int sz = lengths[i+1] - lengths[i];
        assert(sz > 0);
        return sz;
    }

    sat_n::Lit ClauseList::clauseLit(int cl, int idx) const
    {
        assert(idx < (int) clauseLen(cl));
        assert(cl >= 0);
        assert(cl < (int) lengths.size());

        int pos = lengths[cl] + idx;
        assert(pos < (int) clauses.size());

        return clauses[pos];
    }
    std::ostream& ClauseList::dump_clause(std::ostream& out, int i) const
    {
        using namespace ckt_n;
        int sz = clauseLen(i);
        for(int j=0; j != sz; j++) {
            out << clauseLit(i, j) << " ";
        }
        return out;
    }

    int ClauseList::addRewrittenClauses(
        const std::vector<sat_n::lbool>& assumps, 
        const std::vector<bool>& norewriteVarFlags,
        sat_n::Solver& S)
    {
        int cnt = 0;
        using namespace sat_n;
        std::map<Lit, Lit> rewrites;

        for(unsigned i=0; i != numClauses(); i++) {
            int len = clauseLen(i);
            assert(len > 0);
            vec_lit_t newClause;

            Lit sat_lit;
            bool sat = false;
            for(int j=0; j != len; j++) {
                Lit l_ij = clauseLit(i, j);
                Var v_ij = var(l_ij);
                assert((int) v_ij < (int) assumps.size());
                sat_n::lbool val_ij = assumps[v_ij];
                // the variable is not defined to be either true or false.
                if(val_ij.isUndef()) {
                    assert(v_ij < norewriteVarFlags.size());
                    // don't rewrite it?
                    if(norewriteVarFlags[v_ij]) {
                        newClause.push(l_ij);
                    } else {
                        // otherwise we need to introduce new variables
                        // for these "internal" vars which are from the
                        // second level of existential quantification.
                        if(rewrites.find(l_ij) == rewrites.end()) {
                            Var new_var = S.newVar();

                            Lit new_l_ij = mkLit(new_var);
                            rewrites[mkLit(var(l_ij))] = new_l_ij;
                            rewrites[~mkLit(var(l_ij))] = ~new_l_ij;
                        }
                        newClause.push(rewrites[l_ij]);
                    }
                } else  {
                    assert(val_ij.isDef());
                    sat = (val_ij.getBool() && sign(l_ij) == 0) ||
                          (!val_ij.getBool() && sign(l_ij) == 1);
                    bool unsat = (!val_ij.getBool() && sign(l_ij) == 0) ||
                                  (val_ij.getBool() && sign(l_ij) == 1);
                    if(sat) {
                        sat_lit = l_ij;
                        break;
                    } else {
                        assert(unsat);
                    }
                }
            }
            if(sat) {
                continue;
            }

            // FIXME: might need to remove this assertion if formulas are UNSAT?
            assert(newClause.size() > 0);
            S.addClause(newClause);
            cnt += 1;
        }
        return cnt;
    }

    int ClauseList::addRewrittenClauses(
        const std::vector<sat_n::lbool>& assumps, 
        const std::vector<bool>& norewriteVarFlags,
		const std::map<sat_n::Lit, sat_n::Lit>& renameMap,
        sat_n::Solver& S)
    {
        int cnt = 0;
        using namespace sat_n;
        std::map<Lit, Lit> rewrites;

        for(unsigned i=0; i != numClauses(); i++) {
            int len = clauseLen(i);
            assert(len > 0);
            vec_lit_t newClause;

            Lit sat_lit;
            bool sat = false;
            for(int j=0; j != len; j++) {
                Lit l_ij_in = clauseLit(i, j);
				auto pos = renameMap.find(l_ij_in);
				assert (pos != renameMap.end());

				Lit l_ij = pos->second;
                Var v_ij = var(l_ij);
                assert((int) v_ij < (int) assumps.size());
                sat_n::lbool val_ij = assumps[v_ij];
                // the variable is not defined to be either true or false.
                if(val_ij.isUndef()) {
                    assert(v_ij < norewriteVarFlags.size());
                    // don't rewrite it?
                    if(norewriteVarFlags[v_ij]) {
                        newClause.push(l_ij);
                    } else {
                        // otherwise we need to introduce new variables
                        // for these "internal" vars which are from the
                        // second level of existential quantification.
                        if(rewrites.find(l_ij) == rewrites.end()) {
                            Var new_var = S.newVar();

                            Lit new_l_ij = mkLit(new_var);
                            rewrites[mkLit(var(l_ij))] = new_l_ij;
                            rewrites[~mkLit(var(l_ij))] = ~new_l_ij;
                        }
                        newClause.push(rewrites[l_ij]);
                    }
                } else  {
                    assert(val_ij.isDef());
                    sat = (val_ij.getBool() && sign(l_ij) == 0) ||
                          (!val_ij.getBool() && sign(l_ij) == 1);
                    bool unsat = (!val_ij.getBool() && sign(l_ij) == 0) ||
                                  (val_ij.getBool() && sign(l_ij) == 1);
                    if(sat) {
                        sat_lit = l_ij;
                        break;
                    } else {
                        assert(unsat);
                    }
                }
            }
            if(sat) {
                continue;
            }

            // FIXME: might need to remove this assertion if formulas are UNSAT?
            assert(newClause.size() > 0);
            S.addClause(newClause);
            cnt += 1;
        }
        return cnt;
    }
    int ClauseList::addRewrittenClauses(
        std::vector<sat_n::lbool>& values,
        const std::vector<bool>& norewriteVarFlags,
        const std::vector<bool>& dualrailVarFlags,
        sat_n::Solver& S_add,
        sat_n::Solver& S_solve,
        std::map<sat_n::Lit, int>& dualRailLits
    )
    {
        int cnt = 0;
        using namespace sat_n;
        std::map<Lit, Lit> rewrites;

        for(unsigned i=0; i != numClauses(); i++) {
            int len = clauseLen(i);
            assert(len > 0);
            vec_lit_t newClause;

            if(verbose) {
                dump_clause(std::cout << "original clause: ", i)  << std::endl;
            }

            Lit sat_lit;
            bool sat = false;
            for(int j=0; j != len; j++) {
                Lit l_ij = clauseLit(i, j);
                Var v_ij = var(l_ij);
                assert(v_ij < values.size());
                sat_n::lbool val_ij = values[v_ij];
                // the variable is not defined to be either true or false.
                if(val_ij.isUndef()) {
                    assert(v_ij < norewriteVarFlags.size());
                    // if it is an existentially quantified variable,
                    // then we don't rewrite it. 
                    assert(v_ij >= 0 && v_ij < norewriteVarFlags.size());
                    if(norewriteVarFlags[v_ij]) {
                        newClause.push(l_ij);
                    } else {
                        // otherwise we need to introduce new variables
                        // for these "internal" vars which are from the
                        // second level of existential quantification.
                        if(rewrites.find(l_ij) == rewrites.end()) {
                            Lit new_l_ij, new_l_ij_inv;

                            // depending on whether this is to be rewritten in
                            // dual rail form we do different things.
                            assert(v_ij >= 0 && v_ij < dualrailVarFlags.size());
                            if(dualrailVarFlags[v_ij]) {
                                using namespace ckt_n;

                                Var new_var_1 = S_add.newVar();
                                Var new_var_2 = S_add.newVar();
                                new_l_ij = mkLit(new_var_1);
                                new_l_ij_inv = mkLit(new_var_2);
                                if(((verbose&CL_VERBOSE) != 0)) {
                                    std::cout << "l_ij = " << l_ij << "; v_ij = " << v_ij << std::endl;
                                    std::cout << "new_var_1 = " << new_var_1 << std::endl;
                                    std::cout << "new_var_2 = " << new_var_2 << std::endl;
                                }

                                lbool val_ij = S_solve.modelValue(v_ij);
                                assert(val_ij.isDef());
                                dualRailLits[new_l_ij] = val_ij.getBool();
                                dualRailLits[new_l_ij_inv] = !val_ij.getBool();

                                if(((verbose&CL_VERBOSE) != 0)) {
                                    std::cout << "dualRailLits.size() = " << dualRailLits.size() << std::endl;
                                }
                                S_add.addClause(~new_l_ij, ~new_l_ij_inv);
                                cnt += 1;
                            } else {
                                Var new_var = S_add.newVar();
                                new_l_ij = mkLit(new_var);
                                new_l_ij_inv = ~new_l_ij;
                            }

                            rewrites[mkLit(var(l_ij))] = new_l_ij;
                            rewrites[~mkLit(var(l_ij))] = new_l_ij_inv;
                        }
                        newClause.push(rewrites[l_ij]);
                    }
                } else  {
                    sat = (val_ij.getBool() && sign(l_ij) == 0) ||
                          (!val_ij.getBool() && sign(l_ij) == 1);
                    bool unsat = (!val_ij.getBool() && sign(l_ij) == 0) ||
                                 (val_ij.getBool() && sign(l_ij) == 1);
                    if(sat) {
                        sat_lit = l_ij;
                        break;
                    } else {
                        assert(unsat);
                    }
                }
            }
            if(sat) {
                if(verbose) {
                    using namespace ckt_n;
                    std::cout << "skipping satisfied clause. lit: " << sat_lit << std::endl;
                }
                continue;
            }

            // FIXME: might need to remove this assertion if formulas are UNSAT?
            assert(newClause.size() > 0);
            if(verbose) {
                using namespace ckt_n;
                ckt_n::dump_clause(std::cout << "adding duplicate clause: ", newClause) << std::endl;
            }
            S_add.addClause(newClause);
            cnt += 1;
        }
        return cnt;
    }
}
