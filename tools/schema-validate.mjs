// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * schema-validate.mjs — a JSON Schema validator for the subset schema/plugin.decl.schema.json uses.
 * A module of its own, so every tool can validate a declaration without loading the completeness checker
 * (tools/plugin-recipe.mjs re-exports it). A `$ref` into the same file is `#/...`; one into omx-panel's lists
 * (the widgets, the indexed roles, visibleWhen) names its schema by the document's name, `panel.schema.json#/...`,
 * and resolves to the installed copy tools/omx-panel.mjs reads, whatever address that copy's `$id` prints: the
 * publisher's address has moved (github.com/FreeMixer, github.com/Open-Mixer in omx-panel 0.13.8, openmixer.org by
 * ruling D21) and a reference by address went unresolved at each move (omx-plugins#98). No other document resolves.
 */
import { PANEL_SCHEMA } from './omx-panel.mjs';

/** The name omx-panel's schema is installed under (schema/panel.schema.json), the one a declaration schema's `$ref` names. */
export const PANEL_DOCUMENT = 'panel.schema.json';

/** Errors of `value` against `schema` (draft 2020-12: $ref into $defs, type, const, enum, pattern,
 * minLength, required, properties, additionalProperties, items, minItems, uniqueItems, anyOf, allOf, oneOf,
 * not, if/then/else, boolean schemas, minimum, maximum, minProperties, propertyNames). */
export function validate(root, schema, value, path = '') {
  if (schema === true) return [];
  if (schema === false) return [`${path || '/'}: is not allowed here`];
  const errs = [];
  const err = (msg) => errs.push(`${path || '/'}: ${msg}`);
  if (schema.$ref) {
    const [base, pointer = ''] = schema.$ref.split('#');
    const from = base === '' ? root : base === PANEL_DOCUMENT ? PANEL_SCHEMA : undefined;
    const target = from && pointer.replace(/^\//, '').split('/').filter(Boolean).reduce((n, k) => n?.[k], from);
    if (!target) return [`${path || '/'}: unresolved ${schema.$ref}`];
    errs.push(...validate(from, target, value, path));
  }
  const type = Array.isArray(value) ? 'array' : value === null ? 'null' : typeof value;
  if (schema.type) {
    const ok = schema.type === 'integer' ? Number.isInteger(value) : schema.type === type;
    if (!ok) return [...errs, `${path || '/'}: is ${type}, not ${schema.type}`];
  }
  if ('const' in schema && JSON.stringify(value) !== JSON.stringify(schema.const)) err(`must be ${JSON.stringify(schema.const)}`);
  if (schema.enum && !schema.enum.includes(value)) err(`must be one of ${schema.enum.join(', ')}`);
  if (typeof value === 'number') {
    if (typeof schema.minimum === 'number' && value < schema.minimum) err(`${value} is below ${schema.minimum}`);
    if (typeof schema.maximum === 'number' && value > schema.maximum) err(`${value} is above ${schema.maximum}`);
  }
  if (typeof value === 'string') {
    if (schema.pattern && !new RegExp(schema.pattern).test(value)) err(`'${value}' does not match ${schema.pattern}`);
    if (schema.minLength && value.length < schema.minLength) err(`shorter than ${schema.minLength}`);
  }
  if (type === 'object') {
    for (const k of schema.required ?? []) if (!(k in value)) err(`'${k}' is required`);
    if (schema.minProperties && Object.keys(value).length < schema.minProperties) err(`fewer than ${schema.minProperties} properties`);
    if (schema.propertyNames) for (const k of Object.keys(value)) errs.push(...validate(root, schema.propertyNames, k, `${path}/${k}`));
    for (const [k, v] of Object.entries(value)) {
      if (schema.properties && k in schema.properties) errs.push(...validate(root, schema.properties[k], v, `${path}/${k}`));
      else if (schema.additionalProperties === false) err(`'${k}' is not a property`);
      else if (typeof schema.additionalProperties === 'object') errs.push(...validate(root, schema.additionalProperties, v, `${path}/${k}`));
    }
  }
  if (type === 'array') {
    if (schema.minItems && value.length < schema.minItems) err(`fewer than ${schema.minItems} items`);
    if (schema.uniqueItems && new Set(value.map((v) => JSON.stringify(v))).size !== value.length) err('items repeat');
    if (schema.items) value.forEach((v, i) => errs.push(...validate(root, schema.items, v, `${path}/${i}`)));
  }
  for (const sub of schema.allOf ?? []) errs.push(...validate(root, sub, value, path));
  if (schema.oneOf && schema.oneOf.filter((s) => validate(root, s, value, path).length === 0).length !== 1) err(`matches not exactly one of ${schema.oneOf.length} schemas`);
  if (schema.not && validate(root, schema.not, value, path).length === 0) err('matches a schema it must not');
  if (schema.if !== undefined) {
    const holds = validate(root, schema.if, value, path).length === 0;
    if (holds && schema.then !== undefined) errs.push(...validate(root, schema.then, value, path));
    if (!holds && schema.else !== undefined) errs.push(...validate(root, schema.else, value, path));
  }
  if (schema.anyOf && !schema.anyOf.some((s) => validate(root, s, value, path).length === 0)) {
    err(`matches none of: ${schema.anyOf.map((s) => (s.required ? `{${s.required.join(', ')}}` : '…')).join(' or ')}`);
  }
  return errs;
}
