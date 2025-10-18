#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    startTimer(3000);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    currentSampleRate = sampleRate;
    phases.resize(frequencies.size(), 0);
    phaseIncrements.resize(frequencies.size());

    delayBuffer.resize(static_cast<size_t>(sampleRate * 2), 0.0f);
    writeIndex = 0;
    delaySamples = static_cast<int>(sampleRate * 0.3f);
    decay = 0.5f;

    for (size_t i = 0; i < frequencies.size(); ++i) {
        phaseIncrements[i] = 2.0 * juce::MathConstants<double>::pi * frequencies[i] / sampleRate;
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear any unused portion of the buffer (good practice)
    bufferToFill.clearActiveBufferRegion();

    const size_t numOscillators = frequencies.size();
    if (numOscillators == 0)
        return;

    // We'll calculate the mixed output and apply the delay to it
    for (int i = 0; i < bufferToFill.numSamples; ++i) {

        // 1. SYNTHESIS: Calculate the input signal (mixed sine waves)
        float inputSignal = 0.0f;
        for (size_t osc = 0; osc < numOscillators; ++osc) { // <--- NEW OSCILLATOR LOOP

            // Generate sine wave
            inputSignal += std::sin(phases[osc]);

            // Update phase (using double precision for phase increments)
            phases[osc] += phaseIncrements[osc];

            // Wrap phase
            if (phases[osc] >= 2.0 * juce::MathConstants<double>::pi) {
                phases[osc] -= 2.0 * juce::MathConstants<double>::pi;
            }
        }

        // Normalize the synthesized input signal
        inputSignal /= static_cast<float>(numOscillators);

        // --- Delay/Reverb Logic ---

        // Read index for the delayed sample
        int readIndex = (writeIndex + delayBuffer.size() - delaySamples) % delayBuffer.size();
        float delayedSample = delayBuffer[readIndex];

        // 2. PROCESSING: Apply feedback delay
        float outputSignal = inputSignal + delayedSample * decay;

        // Write the output (mixed + delayed) to the audio buffers
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
            auto* channelData = bufferToFill.buffer->getWritePointer(channel, i);
            *channelData = outputSignal;
        }

        // Write the processed signal to the delay buffer
        delayBuffer[writeIndex] = outputSignal;

        // Advance write index (circular buffer)
        writeIndex = (writeIndex + 1) % delayBuffer.size();
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::timerCallback()
{
    stopTimer();
    shutdownAudio();
}
