using Codice.Client.BaseCommands.BranchExplorer;
using Unity.Collections;
using Unity.Mathematics;
using UnityEditor;
using UnityEditor.AssetImporters;
using UnityEngine;
using static Unity.Mathematics.math;

namespace DetailNormal.Textures.Editor
{
    [ScriptedImporter(0, new[] { Extension }, null)]
    public class CustomTextureCreation : ScriptedImporter
    {
        [SerializeField] private float epsilon = 0.01f;
        [SerializeField] private float flatten = 1.0f;
        [SerializeField][Range(0.0f, 1.0f)] private float flatten2 = 0.0f;
        private const string Extension = "ctex";
        private const int Width = 128;

        private float ValueAtUV(float2 uv)
        {
            return (frac(uv.x * 4.0f) + frac(uv.y * 4.0f)) / 2.0f;
        }

        public override void OnImportAsset(AssetImportContext ctx)
        {
            var pixels = new NativeArray<byte>(Width * Width * 4, Allocator.Persistent);
            const float invHeight = 1.0f / Width;
            const float invWidth = 1.0f / Width;
            var e = math.float2(epsilon, 0.0f);
            for (var y = 0; y < Width; ++y)
            {
                var v = y * invHeight;
                for (var x = 0; x < Width; ++x)
                {
                    var u = x * invWidth;
                    var uv = float2(u, v);
                    var p = ValueAtUV(uv);
                    var a = ValueAtUV(uv + e.xy);
                    var b = ValueAtUV(uv + e.yx);
                    var slope = float2(a - p, b - p);
                    var z = flatten - dot(slope.xy, slope.xy);
                    var n = float3(slope * 0.5f + 0.5f, z);
                    var detailNormal = normalize(lerp(n, float3(0.0f, 0.0f, 1.0f), flatten2));

                    pixels[(y * Width + x) * 4 + 0] = (byte)(saturate(detailNormal.x) * 255.0f);
                    pixels[(y * Width + x) * 4 + 1] = (byte)(saturate(detailNormal.y) * 255.0f);
                    pixels[(y * Width + x) * 4 + 2] = (byte)(saturate(detailNormal.z) * 255.0f);
                    pixels[(y * Width + x) * 4 + 3] = 255;
                }
            }

            var texture = new Texture2D(Width, Width, TextureFormat.BGRA32, true, true);
            texture.name = "result texture";
            texture.SetPixelData(pixels, 0);
            texture.Apply(true, true);
            ctx.AddObjectToAsset("texture", texture);
        }

        //[MenuItem("Shader/New Custom Texture")]
        private static void CreateNewAsset()
        {
            ProjectWindowUtil.CreateAssetWithContent($"new custom texture.{Extension}", ".");
        }
 
     }
 }
