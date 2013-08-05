#ifndef READSET_H
#define READSET_H

#include <G3D/TextInput.h>

template <typename Set>
void readSet(G3D::TextInput& ti, Set& set) {
    set.clear();

    G3D::Token t;
    ti.read(t);
    debugAssert(t.type() == G3D::Token::SYMBOL);
    debugAssert(t.string() == "{");

    while (true) {
        ti.read(t);
        if (t.string() == "}") {
            break;
        }
        debugAssert(t.string() == "(");

        int i = ti.readInteger();

        ti.read(t);
        debugAssert(t.string() == ",");

        int j = ti.readInteger();

        set.add(i,j);

        ti.read(t);
        debugAssert(t.string() == ")");

        if (ti.peek().string() == ",") {
            ti.read(t);  // skip
        }
    }
}

#endif  // READSET_H
