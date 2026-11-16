// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
// GENERATED — DO NOT EDIT BY HAND. Produced by tools/modgui-gen.mjs from
// plugins/omx-delay/omx-delay.decl.json (its panel block) and tools/look.json
// Regenerate: `node tools/modgui-gen.mjs`, then commit the result.
function (event, funcs) {
    // a toggled port reads ON or OFF: mod-ui writes the number, the script writes the word after it
    var onoff = function (symbol, value) {
        event.icon.find('[data-onoff][mod-port-symbol="' + symbol + '"]').each(function () {
            this.textContent = value >= 0.5 ? 'ON' : 'OFF';
        });
    };
    // the look: the pedal wears the class its template names (the shipped default), unless the browser remembers another; a key swaps it.
    // Every storage access is guarded: with storage absent or throwing the pedal wears the shipped default and the keys still work.
    var LOOKS = ['vintage', 'console', '3d'];
    var found = event.icon.find('[data-omx-pedal]');
    var pedal = found && found[0];
    var wear = function (name) {
        if (!pedal || !pedal.classList) return;
        for (var i = 0; i < LOOKS.length; i++) pedal.classList.remove('omx-look-' + LOOKS[i]);
        pedal.classList.add('omx-look-' + name);
    };
    var remember = function () {
        var saved = null;
        try { saved = localStorage.getItem('omx-look'); } catch (e) { saved = null; }
        if (LOOKS.indexOf(saved) >= 0) wear(saved);
        event.icon.find('[data-look]').each(function () {
            if (this.omxBound) return;
            this.omxBound = true;
            var name = this.getAttribute('data-look');
            var stop = function (e) { e.stopPropagation(); };
            this.addEventListener('mousedown', stop);
            this.addEventListener('touchstart', stop);
            this.addEventListener('click', function (e) {
                e.stopPropagation();
                wear(name);
                try { localStorage.setItem('omx-look', name); } catch (err) { /* the look is worn for this session only */ }
            });
        });
    };
    // the needles: an output port's value is the target of the meter that names it; a timer lags the needle towards it by the declared
    // time constant (a first-order lag, value += (target - value) * (1 - exp(-dt / tau))), so a level rises and falls like a VU
    var angle = function (d, value) {
        var min = parseFloat(d.getAttribute('data-min')), max = parseFloat(d.getAttribute('data-max'));
        var a0 = parseFloat(d.getAttribute('data-angle-min')), a1 = parseFloat(d.getAttribute('data-angle-max'));
        var clamp = function (x) { return x < 0 ? 0 : x > 1 ? 1 : x; };
        if (d.getAttribute('data-reversed') === '1') return a1 - clamp((max - value) / (max - min)) * (a1 - a0);
        return a0 + clamp((value - min) / (max - min)) * (a1 - a0);
    };
    var draw = function (d) {
        var needle = d.querySelector('.omx-vu-needle');
        if (needle) needle.style.transform = 'rotate(' + angle(d, d.omxVu.cur).toFixed(2) + 'deg)';
    };
    var tick = function () {
        var now = Date.now();
        event.icon.find('[data-vu]').each(function () {
            var v = this.omxVu;
            if (!v) return;
            var dt = now - v.t;
            v.t = now;
            v.cur += (v.target - v.cur) * (1 - Math.exp(-dt / parseFloat(this.getAttribute('data-tau'))));
            draw(this);
        });
    };
    var vuSet = function (symbol, value, jump) {
        event.icon.find('[data-vu]').each(function () {
            if (this.getAttribute('data-vu-symbol') !== symbol) return;
            if (!this.omxVu) this.omxVu = { cur: jump ? value : (this.getAttribute('data-reversed') === '1' ? parseFloat(this.getAttribute('data-max')) : parseFloat(this.getAttribute('data-min'))), target: value, t: Date.now() };
            this.omxVu.target = value;
            this.omxVu.t = this.omxVu.t || Date.now();
            draw(this);
        });
        if (pedal && !pedal.omxTimer && typeof setInterval === 'function') {
            var first = event.icon.find('[data-vu]')[0];
            pedal.omxTimer = setInterval(tick, first ? parseFloat(first.getAttribute('data-step')) : 30);
        }
    };
    var show = function (symbol, value) {
        vuSet(symbol, value, event.type === 'start');
        onoff(symbol, value);
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
        var found = event.icon.find('[data-omx-pedal]');
        var root = found && found[0];
        if (!root || !root.querySelectorAll) return;
        var n = function (name) { return parseFloat(root.getAttribute(name)); };
        var y = n('data-section-top');
        var sections = root.querySelectorAll('.omx-section');
        var wide = n('data-base-w');
        for (var s = 0; s < sections.length; s++) {
            var sec = sections[s];
            if (sec.className.indexOf('omx-meters') >= 0) {
                sec.style.top = y + 'px';
                y += parseFloat(sec.style.height) + n('data-section-gap');
                continue;
            }
            var curves = parseFloat(sec.getAttribute('data-curves')) || 0;
            var matrix = sec.getAttribute('data-matrix') === '1';
            var col = 0, row = 0, prev = null, any = false, cells = [];
            for (var c = 0; c < sec.children.length; c++) {
                var k = sec.children[c];
                if (k.className.indexOf('omx-control') < 0 || k.style.display === 'none') continue;
                if (matrix && k.getAttribute('data-mx-col')) { cells.push(k); continue; }
                var band = k.getAttribute('data-band') || '';
                var span = parseFloat(k.getAttribute('data-span')) || 1;
                if (any && (col + span > n('data-row-controls') || band !== prev)) { col = 0; row++; }
                prev = band; any = true;
                k.style.left = (n('data-section-pad') + col * n('data-cell-w')) + 'px';
                k.style.top = (n('data-title-h') + curves + row * n('data-cell-h')) + 'px';
                col += span;
            }
            var top = n('data-title-h') + curves + (any ? row + 1 : 0) * n('data-cell-h');
            var h = top + n('data-section-pad') / 2;
            if (matrix) {
                // an aligned matrix: a column per band that has a shown control, in the order the template lists them (HPF, the bands, LPF),
                // each parameter on its row, the same y in every column
                var order = [], at = {};
                for (var m = 0; m < cells.length; m++) {
                    var key = cells[m].getAttribute('data-mx-col');
                    if (!(key in at)) { at[key] = order.length; order.push(key); }
                    cells[m].style.left = (n('data-section-pad') + at[key] * n('data-cell-w')) + 'px';
                    cells[m].style.top = (top + n('data-head-h') + parseFloat(cells[m].getAttribute('data-mx-ri')) * n('data-cell-h')) + 'px';
                }
                for (var c2 = 0; c2 < sec.children.length; c2++) {
                    var head = sec.children[c2];
                    if (head.className.indexOf('omx-colhead') < 0) continue;
                    var hk = head.getAttribute('data-mx-col');
                    head.style.display = hk in at ? '' : 'none';
                    if (hk in at) { head.style.left = (n('data-section-pad') + at[hk] * n('data-cell-w')) + 'px'; head.style.top = (top + 6) + 'px'; }
                }
                if (order.length) h += n('data-head-h') + parseFloat(sec.getAttribute('data-mx-rows')) * n('data-cell-h');
                wide = Math.max(wide, order.length * n('data-cell-w') + 2 * n('data-section-pad'));
            }
            sec.style.top = y + 'px';
            sec.style.height = h + 'px';
            y += h + n('data-section-gap');
        }
        // the face is as wide as its widest section needs: a matrix of more bands widens every section, its curve and the pedal
        for (var w = 0; w < sections.length; w++) {
            sections[w].style.width = wide + 'px';
            var curve = sections[w].querySelectorAll('.omx-curve');
            for (var q = 0; q < curve.length; q++) curve[q].style.width = (wide - 2 * n('data-section-pad')) + 'px';
        }
        root.style.width = (wide + 2 * n('data-pad')) + 'px';
        root.style.height = (y - n('data-section-gap') + n('data-pad')) + 'px';
    };
    if (event.type === 'start') {
        remember();
        for (var i = 0; i < event.ports.length; i++) show(event.ports[i].symbol, event.ports[i].value);
        relayout();
    } else if (event.type === 'change') {
        show(event.symbol, event.value);
        relayout();
    }
}
