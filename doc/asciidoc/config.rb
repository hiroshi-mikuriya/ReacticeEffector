# frozen_string_literal: true

require 'prawn-svg'
Prawn::Svg::Font::GENERIC_CSS_FONT_MAPPING.merge!(
  'sans-serif' => 'KaiGen Gothic JP'
)
