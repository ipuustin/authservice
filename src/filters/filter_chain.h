#ifndef AUTHSERVICE_FILTER_CHAIN_H
#define AUTHSERVICE_FILTER_CHAIN_H

#include <memory>

#include "config/config.pb.h"
#include "envoy/service/auth/v3/external_auth.grpc.pb.h"
#include "src/filters/filter.h"
#include "src/filters/oidc/session_store.h"

namespace authservice {
namespace filters {
/**
 * FilterChain is an object that wraps a Pipe and the criteria for asserting
 * whether a Pipe should process a request.
 */
class FilterChain {
 public:
  virtual ~FilterChain() = default;

  /**
   * Name returns a name given to the filter chain for use in debugging and
   * logging.
   * @return the name of the filter chain.
   */
  virtual const std::string &Name() const = 0;

  /**
   * Matches can be used to identify whether a chain should be used to process a
   * request.
   * @param request the request to match against.
   * @return true if that chain should process a request else false.
   */
  virtual bool Matches(
      const ::envoy::service::auth::v3::CheckRequest *request) const = 0;

  /**
   * New creates a new filter instance that can be used to process a request.
   * @return a new filter instance.
   */
  virtual std::unique_ptr<Filter> New() = 0;

  /**
   * Invoked periodically to give the filter chain a chance to clean up expired
   * sessions and any other resources.
   */
  virtual void DoPeriodicCleanup() = 0;
};

class FilterChainImpl : public FilterChain {
 private:
  unsigned int threads_;
  config::FilterChain config_;
  std::shared_ptr<oidc::SessionStore> oidc_session_store_;
  config::oidc::OIDCConfig default_oidc_config_;

 public:
  explicit FilterChainImpl(config::FilterChain config, unsigned int threads);
  explicit FilterChainImpl(config::oidc::OIDCConfig default_oidc_config,
                           config::FilterChain config, unsigned int threads);

  const std::string &Name() const override;

  bool Matches(
      const ::envoy::service::auth::v3::CheckRequest *request) const override;

  std::unique_ptr<Filter> New() override;

  virtual void DoPeriodicCleanup() override;

  const config::FilterChain &Config() const & { return config_; }
};

}  // namespace filters
}  // namespace authservice
#endif  // AUTHSERVICE_FILTER_CHAIN_H
