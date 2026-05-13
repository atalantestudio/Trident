// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIUTF8Layer : public GUILayer {
		public:
			void registerAssets() override {
				const view<char8> assetName = getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName");

				font = loadFontAsset(assetName);
			}

			void render() override {
				const TextStyle textStyle = {
					.color = 0xffffffff,
					.font = font,
					.fontSize = 96_px,
				};

				// Text("ÀÁÂÃÄÅĀĂĄǍÆǼÇĆĈĊČĎĐÈÉÊËĒĔĖ\nĘĚĜĞĠĢĤĦÌÍÎÏĨĪĬĮİǏĴĶĹĻĽŁÑŃŅŇÒÓÔ\nÕÖØŌŎŐǑŒŔŖŘŚŜŞŠŢŤŦÙÚÛÜŨŪ\nŬŮŰŲǓŴÝŶŸŹŻŽ", textStyle);
				Text("àáâãäåāăąǎæǽçćĉċčďđèéêëēĕėęěĝğ\nġģĥħìíîïĩīĭįıǐĵķĺļľłñńņňòóôõöøōŏőǒœŕŗř\nśŝşšţťŧùúûüũūŭůűųǔŵýŷÿźżž", textStyle);
			}

		private:
			FontID font;
	};
}