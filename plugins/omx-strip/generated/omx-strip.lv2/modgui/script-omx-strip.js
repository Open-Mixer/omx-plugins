// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
// GENERATED — DO NOT EDIT BY HAND. Produced by tools/modgui-gen.mjs from
// plugins/omx-strip/omx-strip.decl.json (its panel block) and tools/look.json
// Regenerate: `node tools/modgui-gen.mjs`, then commit the result.
function (event, funcs) {
    var show = function (symbol, value) {
        var v = Math.round(value);
        event.icon.find('[data-visible-port="' + symbol + '"]').each(function () {
            // the mask is 64 bits of hex: the half that holds bit v, read as a plain number
            var hex = this.getAttribute('data-visible-mask');
            while (hex.length < 16) hex = '0' + hex;
            var word = parseInt(v < 32 ? hex.slice(8) : hex.slice(0, 8), 16);
            var on = v >= 0 && v <= 63 && Math.floor(word / Math.pow(2, v % 32)) % 2 === 1;
            this.style.display = on ? '' : 'none';
        });
    };
    // a control that is hidden takes no room: the sections are laid out again over the shown controls, as the generator lays out
    // the come-up face (the same grid: a row holds data-row-controls controls, an edge band starts a row of its own)
    var relayout = function () {
        var found = event.icon.find('.omx-pedal');
        var root = found && found[0];
        if (!root || !root.querySelectorAll) return;
        var n = function (name) { return parseFloat(root.getAttribute(name)); };
        var y = n('data-section-top');
        var sections = root.querySelectorAll('.omx-section');
        for (var s = 0; s < sections.length; s++) {
            var sec = sections[s];
            if (sec.className.indexOf('omx-meters') >= 0) {
                sec.style.top = y + 'px';
                y += parseFloat(sec.style.height) + n('data-section-gap');
                continue;
            }
            var curves = parseFloat(sec.getAttribute('data-curves')) || 0;
            var col = 0, row = 0, prev = null, any = false;
            for (var c = 0; c < sec.children.length; c++) {
                var k = sec.children[c];
                if (k.className.indexOf('omx-control') < 0 || k.style.display === 'none') continue;
                var band = k.getAttribute('data-band') || '';
                if (any && (col === n('data-row-controls') || band !== prev)) { col = 0; row++; }
                prev = band; any = true;
                k.style.left = (n('data-section-pad') + col * n('data-cell-w')) + 'px';
                k.style.top = (n('data-title-h') + curves + row * n('data-cell-h')) + 'px';
                col++;
            }
            var h = n('data-title-h') + curves + (any ? row + 1 : 0) * n('data-cell-h') + n('data-section-pad') / 2;
            sec.style.top = y + 'px';
            sec.style.height = h + 'px';
            y += h + n('data-section-gap');
        }
        root.style.height = (y - n('data-section-gap') + n('data-pad')) + 'px';
    };
    if (event.type === 'start') {
        for (var i = 0; i < event.ports.length; i++) show(event.ports[i].symbol, event.ports[i].value);
        relayout();
    } else if (event.type === 'change') {
        show(event.symbol, event.value);
        relayout();
    }
}
