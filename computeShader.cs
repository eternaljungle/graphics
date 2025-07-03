using Unity.Collections;
using UnityEngine;
using UnityEngine.Rendering;

namespace ComputeShaderExample
{
    public class ComputeShaderExample : MonoBehaviour
    {
        [SerializeField] private UnityEngine.ComputeShader compute;

        private int[] _data;
        private GraphicsBuffer _dataBuffer;

        private void Onable()
        {
            _data = new int[5];
            _data[0] = -10;
            _dataBuffer = new GraphicsBuffer(GraphicsBuffer.Target.Raw, 1, 1);
            //count -> raw, bytebuffer 쓸 거라 애매함. databuffer의 크기 count*stride

            _dataBuffer.SetData(_data);
            compute.SetBuffer(0, "_data", _dataBuffer);
            compute.Dispatch(0, 1, 1, 1);

            var req = AsyncGPUReadback.Request(_dataBuffer);
            req.WaitForCompletion();
        }
    }
}

computeShader.compute
#pragma kernel Excute

RWByteAddressBuffer _data;

[numthreads(1, 1, 1)]
void Excute ()
{
    _data.Store3(0, asuint(float3(5.0f, -1.0f, 5.0f))); //저장할 때는 integer로
    _data.Store3(1, ~~) //uint = 4byte, 1이라 쓰면... float-32bit / half-16bit 
    //정밀도에 따라 얼마나 공간을 사용할 것인지 컨트롤 가능

    const float3 position = asfloat(_data.Load3(0)); //불러올 때는 float
}
